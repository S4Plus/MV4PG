/**
 * Copyright 2024 AntGroup CO., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

//
// Created by wt on 18-8-14.
//
#include "./antlr4-runtime.h"
#include "geax-front-end/ast/AstNode.h"
#include "geax-front-end/ast/AstDumper.h"
#include "geax-front-end/isogql/GQLResolveCtx.h"
#include "geax-front-end/isogql/GQLAstVisitor.h"
#include "geax-front-end/isogql/parser/AntlrGqlParser.h"

#include "tools/lgraph_log.h"
#include "core/task_tracker.h"

#include "parser/generated/LcypherLexer.h"
#include "parser/generated/LcypherParser.h"
#include "parser/cypher_base_visitor.h"
#include "parser/cypher_error_listener.h"
#include "parser/trans_single_match_visitor.h"

#include "cypher/execution_plan/execution_plan.h"
#include "cypher/execution_plan/scheduler.h"
#include "cypher/execution_plan/execution_plan_v2.h"
#include "cypher/rewriter/GenAnonymousAliasRewriter.h"

#include "server/bolt_session.h"

namespace cypher {

void Scheduler::Eval(RTContext *ctx, const lgraph_api::GraphQueryType &type,
                     const std::string &script, ElapsedTime &elapsed) {
    if (type == lgraph_api::GraphQueryType::CYPHER) {
        EvalCypher(ctx, script, elapsed);
    } else {
        EvalGql(ctx, script, elapsed);
    }
}

bool Scheduler::DetermineReadOnly(cypher::RTContext *ctx,
                                  const lgraph_api::GraphQueryType &query_type,
                                  const std::string &script, std::string &name, std::string &type) {
    if (query_type == lgraph_api::GraphQueryType::CYPHER) {
        return DetermineCypherReadOnly(ctx, script, name, type);
    } else {
        return DetermineGqlReadOnly(ctx, script, name, type);
    }
}

void Scheduler::WriteProfile(std::string output_dir,std::string cypher_query, std::string content){
    if (!std::filesystem::exists(output_dir)) {
        std::filesystem::create_directories(output_dir);
    }
    std::string filename = output_dir+"/profile_origin.txt";
    std::ofstream outfile(filename, std::ios::app);
    if (!outfile) {
        LOG_DEBUG() << "无法打开文件: " << filename;
        return;
    }
    if(cypher_query!=""){
        outfile << "cypher: " << cypher_query << std::endl;
        outfile << content<<"\n" <<std::endl;
    }
    outfile.close();
}

void Scheduler::EvalCypher(RTContext *ctx, const std::string &script, ElapsedTime &elapsed) {
    ctx->path_unique_ = false;
    using namespace parser;
    using namespace antlr4;
    auto t0 = fma_common::GetTime();
    ANTLRInputStream inputT(script);
    LcypherLexer lexerT(&inputT);
    CommonTokenStream tokensT(&lexerT);
    LOG_DEBUG() <<"parser s"<<std::endl; // de
    LcypherParser parserT(&tokensT);
    TransSingleMatchVisitor visitorT(parserT.oC_Cypher());
    std::string trans_script=visitorT.GetRewriteQuery();
    // <script, execution plan>
    thread_local LRUCacheThreadUnsafe<std::string, std::shared_ptr<ExecutionPlan>> tls_plan_cache;
    std::shared_ptr<ExecutionPlan> plan;
    if (!tls_plan_cache.Get(trans_script, plan)) {
        ANTLRInputStream input(trans_script);
        LcypherLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        LcypherParser parser(&tokens);
        /* We can set ErrorHandler here.
         * setErrorHandler(std::make_shared<BailErrorStrategy>());
         * add customized ErrorListener  */
        parser.addErrorListener(&CypherErrorListener::INSTANCE);
        CypherBaseVisitor visitor(ctx, parser.oC_Cypher());
        LOG_DEBUG() << "-----CLAUSE TO STRING-----";
        for (const auto &sql_query : visitor.GetQuery()) {
            LOG_DEBUG() << sql_query.ToString();
        }
        plan = std::make_shared<ExecutionPlan>();
        plan->Build(visitor.GetQuery(), visitor.CommandType(), ctx, visitor.CommandType()==parser::CmdType::PROFILE);
        plan->Validate(ctx);
        if (plan->CommandType() != parser::CmdType::QUERY
            && plan->CommandType() != parser::CmdType::PROFILE) {
            ctx->result_info_ = std::make_unique<ResultInfo>();
            ctx->result_ = std::make_unique<lgraph::Result>();
            std::string header, data;
            if (plan->CommandType() == parser::CmdType::EXPLAIN) {
                header = "@plan";
                data = plan->DumpPlan(0, false);
            } 
            // else {
            //     header = "@profile";
            //     data = plan->DumpGraph();
            // }
            ctx->result_->ResetHeader({{header, lgraph_api::LGraphType::STRING}});
            auto r = ctx->result_->MutableRecord();
            r->Insert(header, lgraph::FieldData(data));
            if (ctx->bolt_conn_) {
                auto session = (bolt::BoltSession *)ctx->bolt_conn_->GetContext();
                while (!session->streaming_msg) {
                    session->streaming_msg = session->msgs.Pop(std::chrono::milliseconds(100));
                    if (ctx->bolt_conn_->has_closed()) {
                        LOG_INFO() << "The bolt connection is closed, cancel the op execution.";
                        return;
                    }
                }
                std::unordered_map<std::string, std::any> meta;
                meta["fields"] = ctx->result_->BoltHeader();
                bolt::PackStream ps;
                ps.AppendSuccess(meta);
                if (session->streaming_msg.value().type == bolt::BoltMsg::PullN) {
                    ps.AppendRecords(ctx->result_->BoltRecords());
                } else if (session->streaming_msg.value().type == bolt::BoltMsg::DiscardN) {
                    // ...
                }
                ps.AppendSuccess();
                ctx->bolt_conn_->PostResponse(std::move(ps.MutableBuffer()));
            }
            return;
        }
        LOG_DEBUG() << "Plan cache disabled.";
    }
    LOG_DEBUG() << plan->DumpPlan(0, false);
    LOG_DEBUG() << plan->DumpGraph();
    elapsed.t_compile = fma_common::GetTime() - t0;
    if (!plan->ReadOnly() && ctx->optimistic_) {
        while (1) {
            try {
                plan->Execute(ctx);
                break;
            } catch (lgraph::TxnCommitException &e) {
                LOG_DEBUG() << e.what();
            }
        }
    } else {
        plan->Execute(ctx);
    }
    if (plan->CommandType() == parser::CmdType::PROFILE){
        size_t db_hit=plan->GetDBHit();
        std::string profile_inf=plan->DumpPlan(0, true);
        profile_inf="total db hit:"+std::to_string(db_hit)+"\n"+profile_inf;
        auto parent_dir=ctx->galaxy_->GetConfig().dir;
        if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
        WriteProfile(parent_dir+"/output",script,profile_inf);
        size_t result_num=plan->Root()->stats.profileRecordCount;
        ctx->result_info_ = std::make_unique<ResultInfo>();
        ctx->result_ = std::make_unique<lgraph::Result>();

        ctx->result_->ResetHeader({{"db_hit", lgraph_api::LGraphType::STRING},{"result_num", lgraph_api::LGraphType::STRING}});
        auto r = ctx->result_->MutableRecord();
        r->Insert("db_hit", lgraph::FieldData(std::to_string(db_hit)));
        r->Insert("result_num", lgraph::FieldData(std::to_string(result_num)));
    }
    elapsed.t_total = fma_common::GetTime() - t0;
    elapsed.t_exec = elapsed.t_total - elapsed.t_compile;
}

void Scheduler::EvalGql(RTContext *ctx, const std::string &script, ElapsedTime &elapsed) {
    using geax::frontend::GEAXErrorCode;
    auto t0 = fma_common::GetTime();
    std::string result;
    geax::frontend::AntlrGqlParser parser(script);
    parser::GqlParser::GqlRequestContext *rule = parser.gqlRequest();
    if (!parser.error().empty()) {
        LOG_DEBUG() << "parser.gqlRequest() error: " << parser.error();
        result = parser.error();
        throw lgraph::ParserException(result);
    }
    geax::common::ObjectArenaAllocator objAlloc_;
    geax::frontend::GQLResolveCtx gql_ctx{objAlloc_};
    geax::frontend::GQLAstVisitor visitor{gql_ctx};
    rule->accept(&visitor);
    auto ret = visitor.error();
    if (ret != GEAXErrorCode::GEAX_SUCCEED) {
        LOG_DEBUG() << "rule->accept(&visitor) ret: " << ToString(ret);
        result = ToString(ret);
        throw lgraph::GqlException(result);
    }
    geax::frontend::AstNode *node = visitor.result();
    // rewrite ast
    cypher::GenAnonymousAliasRewriter gen_anonymous_alias_rewriter;
    node->accept(gen_anonymous_alias_rewriter);
    // dump
    geax::frontend::AstDumper dumper;
    ret = dumper.handle(node);
    if (ret != GEAXErrorCode::GEAX_SUCCEED) {
        LOG_DEBUG() << "dumper.handle(node) gql: " << script;
        LOG_DEBUG() << "dumper.handle(node) ret: " << ToString(ret);
        LOG_DEBUG() << "dumper.handle(node) error_msg: " << dumper.error_msg();
        result = dumper.error_msg();
        throw lgraph::GqlException(result);
    } else {
        LOG_DEBUG() << "--- dumper.handle(node) dump ---";
        LOG_DEBUG() << dumper.dump();
    }
    cypher::ExecutionPlanV2 execution_plan_v2;
    ret = execution_plan_v2.Build(node);
    elapsed.t_compile = fma_common::GetTime() - t0;
    if (ret != GEAXErrorCode::GEAX_SUCCEED) {
        LOG_DEBUG() << "build execution_plan_v2 failed: " << execution_plan_v2.ErrorMsg();
        result = execution_plan_v2.ErrorMsg();
        throw lgraph::GqlException(result);
    } else {
        execution_plan_v2.Execute(ctx);
        LOG_DEBUG() << "-----result-----";
        result = ctx->result_->Dump(false);
        LOG_DEBUG() << result;
        elapsed.t_total = fma_common::GetTime() - t0;
        elapsed.t_exec = elapsed.t_total - elapsed.t_compile;
    }
}

bool Scheduler::DetermineCypherReadOnly(cypher::RTContext *ctx, const std::string &script,
                                        std::string &name, std::string &type) {
    using namespace parser;
    using namespace antlr4;
    ANTLRInputStream input(script);
    LcypherLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    LcypherParser parser(&tokens);
    /* We can set ErrorHandler here.
     * setErrorHandler(std::make_shared<BailErrorStrategy>());
     * add customized ErrorListener  */
    parser.addErrorListener(&CypherErrorListener::INSTANCE);
    tree::ParseTree *tree = parser.oC_Cypher();
    CypherBaseVisitor visitor = CypherBaseVisitor(ctx, tree);
    for (const auto &sq : visitor.GetQuery()) {
        if (!sq.ReadOnly(name, type)) return false;
    }
    return true;
}

bool Scheduler::DetermineGqlReadOnly(cypher::RTContext *ctx, const std::string &script,
                                     std::string &name, std::string &type) {
    return true;
}

}  // namespace cypher
