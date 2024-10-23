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
#include "parser/rewrite_views_visitor.h"
#include "parser/reverse_rewrite_visitor.h"
#include "parser/rewrite_use_views_visitor.h"
#include "parser/view_maintenance_visitor.h"

#include "cypher/execution_plan/execution_plan.h"
#include "cypher/execution_plan/scheduler.h"
#include "cypher/execution_plan/execution_plan_v2.h"
#include "cypher/rewriter/GenAnonymousAliasRewriter.h"

#include "server/bolt_session.h"

#define MaintenanceTemplate std::tuple<std::string,std::string,std::string>

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

void AddView(std::string file_path, std::string view_name, std::string start_node_label, std::string end_node_label, std::string query,size_t db_hit,size_t result_num,MaintenanceTemplate templates){
    std::ifstream ifs(file_path);

    // 检查文件是否成功打开
    if (!ifs) {
        std::cout << "File does not exist, creating new file: " << file_path<<std::endl;
        std::ofstream ofs(file_path);
        ofs.close();
        ifs.open(file_path);
        if (!ifs) {
            std::cout << "Failed to open file: " << file_path<<std::endl;
            return;
        }
    }
    LOG_DEBUG()<<"Add view S";
    // 使用nlohmann的json库来解析文件
    nlohmann::json j;
    try {
        ifs >> j;
    } catch (nlohmann::json::parse_error& e) {
        j = nlohmann::json::array();
    }
    ifs.close();
    double opt_rate=0;
    if(result_num>0)
        opt_rate=static_cast<double>(db_hit)/(2*result_num);
    nlohmann::json new_element = {
        {view_name, {
            {"start_node_label", start_node_label},
            {"end_node_label", end_node_label},
            {"query", query},
            {"opt_rate",opt_rate},
            {"result_num",result_num},
            {"delete_vertex",std::get<0>(templates)},
            {"create_edge",std::get<1>(templates)},
            {"delete_edge",std::get<2>(templates)}
        }}
    };

    if(j.size()==0){
        j.push_back(new_element);
    }
    else{
        j[0][view_name] = new_element[view_name];
        // j[0].emplace_back(new_element);
    }
    // j.push_back(final_element);
    std::ofstream ofs(file_path);
    ofs << std::setw(4) << j;
    ofs.close();
    LOG_DEBUG()<<"Add view E";
}


const std::string RewriteCypherUseViews(RTContext *ctx,const std::string &script){
    auto parent_dir=ctx->galaxy_->GetConfig().dir;
    if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
    std::string file_path=parent_dir+"/view/"+ctx->graph_+".json";
    std::ifstream ifs(file_path);

    // 检查文件是否成功打开
    if (!ifs) {
        LOG_DEBUG() << "Failed to open file: " << file_path;
        return script;
    }

    // 使用nlohmann的json库来解析文件
    nlohmann::json j;
    try {
        ifs >> j;
    } catch (nlohmann::json::parse_error& e) {
        j = nlohmann::json::array();
    }
    for(auto element:j){
        std::string view_name = element["view_name"];
        std::string start_node_label = element["start_node_label"];
        std::string end_node_label = element["end_node_label"];
        std::string query = element["query"];
        parser::RewriteUseViewsVisitor visitor(view_name,query,script);
        return visitor.GetRewriteQuery();
    }
    return script;
}

MaintenanceTemplate GeneratorMaintenanceTemplate(std::string view_query){
    using namespace parser;
    using namespace antlr4;
    std::string deleteVertex,createEdge,deleteEdge;
    NODE_INF src("$SL","$SK","$SV",false);
    NODE_INF dst("$DL","$DK","$DV",false);

    ANTLRInputStream inputDV(view_query);
    LcypherLexer lexerDV(&inputDV);
    CommonTokenStream tokensDV(&lexerDV);
    // std::cout <<"parser s1"<<std::endl; // de
    LcypherParser parserDV(&tokensDV);
    ViewMaintenance visitorDV(parserDV.oC_Cypher(),"$L","$K","$V",false,false);
    deleteVertex=visitorDV.GetRewriteQuery();

    ANTLRInputStream inputCE(view_query);
    LcypherLexer lexerCE(&inputCE);
    CommonTokenStream tokensCE(&lexerCE);
    // std::cout <<"parser s1"<<std::endl; // de
    LcypherParser parserCE(&tokensCE);
    ViewMaintenance visitorCE(parserCE.oC_Cypher(),"VR","$RID",src,dst,true);
    createEdge=visitorCE.GetRewriteQuery();

    ANTLRInputStream inputDE(view_query);
    LcypherLexer lexerDE(&inputDE);
    CommonTokenStream tokensDE(&lexerDE);
    // std::cout <<"parser s1"<<std::endl; // de
    LcypherParser parserDE(&tokensDE);
    ViewMaintenance visitorDE(parserDE.oC_Cypher(),"VR","$RID",src,dst,false);
    deleteEdge=visitorDE.GetRewriteQuery();
    MaintenanceTemplate templates=std::make_tuple(deleteVertex,createEdge,deleteEdge);
    return templates;
}

const std::string Scheduler::EvalCypher(RTContext *ctx, const std::string &script, ElapsedTime &elapsed, bool is_with_new_txn) {
    ctx->path_unique_ = false;
    // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist:"<<(ctx->txn_!=nullptr);
    auto parent_dir=ctx->galaxy_->GetConfig().dir;
    if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
    std::string folder=parent_dir+"/view";
    if (!std::filesystem::exists(folder)) {
        std::filesystem::create_directories(folder);
    }

    using namespace parser;
    using namespace antlr4;
    // LOG_DEBUG()<<"tugraph dir: "<< ctx->galaxy_->GetConfig().dir;
    auto t0 = fma_common::GetTime();
    // <script, execution plan>
    thread_local LRUCacheThreadUnsafe<std::string, std::shared_ptr<ExecutionPlan>> tls_plan_cache;
    std::shared_ptr<ExecutionPlan> plan;
    if (!tls_plan_cache.Get(script, plan)) {
        // auto rewrite_query=RewriteCypherUseViews(ctx,script);
        ANTLRInputStream input(script);
        LcypherLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        LOG_DEBUG() <<"parser s"<<std::endl; // de
        LcypherParser parser(&tokens);
        /* We can set ErrorHandler here.
         * setErrorHandler(std::make_shared<BailErrorStrategy>());
         * add customized ErrorListener  */
        LOG_DEBUG() <<"parser e"<<std::endl; // de
        parser.addErrorListener(&CypherErrorListener::INSTANCE);
        LOG_DEBUG() <<"visitor s"<<std::endl; // de
        auto oc_cypher=parser.oC_Cypher();
        CypherBaseVisitor visitor(ctx, oc_cypher);
        // if(!visitor.IsValid()) return std::string();
        // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist2:"<<(ctx->txn_!=nullptr);
        LOG_DEBUG() <<"visitor e"<<std::endl; // de
        LOG_DEBUG() << "-----CLAUSE TO STRING-----";
        for (const auto &sql_query : visitor.GetQuery()) {
            LOG_DEBUG() << sql_query.ToString();
        }
        plan = std::make_shared<ExecutionPlan>();
        if(visitor.CommandType()==parser::CmdType::MAINTENANCE)
            plan.get()->SetMaintenance(true);
        else if(visitor.CommandType()==parser::CmdType::OPTIMIZE)
            plan.get()->SetOptimize(true);
        else if(visitor.CommandType()==parser::CmdType::VIEW){ //创建视图
            LOG_DEBUG() << "s";
            const std::vector<cypher::PatternGraph>& pattern_graphs=plan->GetPatternGraphs();
            LOG_DEBUG() << "new visitor s";
            RewriteViews new_visitor(ctx,oc_cypher,pattern_graphs);
            LOG_DEBUG() << "new visitor e";
            auto view_name=new_visitor.GetViewName();
            auto constraints=new_visitor.GetConstraints();
            auto new_query=new_visitor.GetRewriteQuery();
            auto profile_new_query="profile "+new_query;
            LOG_DEBUG() << "profile_new_query:"<<profile_new_query;
            // std::string create_query="CALL db.createEdgeLabel('"+view_name+"', '[[\""+constraints.first+"\",\""+constraints.second+"\"]]','is_view',bool,true)";
            std::string create_query="CALL db.createEdgeLabel('"+view_name+"', '[]')";
            // std::cout<<"create query: "<<create_query<<std::endl;
            // std::cout<<"new query: "<<new_query<<std::endl;
            elapsed.t_compile = fma_common::GetTime() - t0;
            ElapsedTime temp;
            if(is_with_new_txn)
                EvalCypher(ctx,create_query,temp);
            else
                EvalCypherWithoutNewTxn(ctx,create_query,temp);
            // 将信息保存到json文件中
            auto parent_dir=ctx->galaxy_->GetConfig().dir;
            if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
            std::string file_path=parent_dir+"/view/"+ctx->graph_+".json";
            MaintenanceTemplate templates=GeneratorMaintenanceTemplate(new_query);
            AddView(file_path,view_name,constraints.first,constraints.second,new_query,0,0,templates);
            // std::string file_path="/data/view/"+ctx->graph_+".json";
            // AddView(file_path,view_name,constraints.first,constraints.second,new_query,0,0);
            
            // 为重写后的语句生成执行计划
            ANTLRInputStream new_input(profile_new_query);
            LcypherLexer new_lexer(&new_input);
            CommonTokenStream new_tokens(&new_lexer);
            LcypherParser new_parser(&new_tokens);
            new_parser.addErrorListener(&CypherErrorListener::INSTANCE);
            auto new_oc_cypher=new_parser.oC_Cypher();
            CypherBaseVisitor new_base_visitor(ctx, new_oc_cypher);
            LOG_DEBUG() <<"visitor e"<<std::endl; // de
            LOG_DEBUG() << "-----CLAUSE TO STRING-----";
            plan = std::make_shared<ExecutionPlan>();
            plan->SetCreateView(true);
            plan->Build(new_base_visitor.GetQuery(), new_base_visitor.CommandType(), ctx,true);
            // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist7:"<<(ctx->txn_!=nullptr);
            plan->Validate(ctx);
            
            if(!is_with_new_txn)
                plan->ExecuteWithoutNewTxn(ctx);
            else{
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
            }
            LOG_DEBUG()<<"create view end";
            // if(is_with_new_txn)
            //     EvalCypher(ctx,new_query,temp);
            // else
            //     EvalCypherWithoutNewTxn(ctx,new_query,temp);
            // produce results->Create->Apply->Argument或Project
            size_t db_hit=plan->GetDBHit(),result_num=plan->Root()->children[0]->children[0]->children[1]->stats.profileRecordCount;
            // std::string result=ctx->result_->Dump(false);
            // LOG_DEBUG()<<"result size:"<<ctx->result_->Header().size();
            // LOG_DEBUG()<<result;
            // if(ctx->result_->Header().size()==2){
            //     std::string result=ctx->result_->Dump(false);
            //     LOG_DEBUG()<<result;
            //     nlohmann::json j = nlohmann::json::parse(result);
            //     auto db_hit_str=j.at("db_hit").get<std::string>();
            //     auto result_num_str=j.at("result_num").get<std::string>();
            //     db_hit=std::stoull(db_hit_str);
            //     result_num=std::stoull(result_num_str);
            // }
            // MaintenanceTemplate templates=GeneratorMaintenanceTemplate(new_query);
            AddView(file_path,view_name,constraints.first,constraints.second,new_query,db_hit,result_num,templates);
            ctx->result_info_ = std::make_unique<ResultInfo>();
            ctx->result_ = std::make_unique<lgraph::Result>();

            ctx->result_->ResetHeader({{"view_name", lgraph_api::LGraphType::STRING},{"start_node_label", lgraph_api::LGraphType::STRING},{"end_node_label", lgraph_api::LGraphType::STRING}});
            auto r = ctx->result_->MutableRecord();
            r->Insert("view_name", lgraph::FieldData(view_name));
            r->Insert("start_node_label", lgraph::FieldData(constraints.first));
            r->Insert("end_node_label", lgraph::FieldData(constraints.second));
            
            
            elapsed.t_total = fma_common::GetTime() - t0;
            LOG_DEBUG() << "end";
            return std::string();
        }
        // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist6:"<<(ctx->txn_!=nullptr);
        LOG_DEBUG()<<"build start";
        plan->Build(visitor.GetQuery(), visitor.CommandType(), ctx,visitor.CommandType()==parser::CmdType::PROFILE);
        plan->SetCypherQuery(script);
        LOG_DEBUG()<<"build end";
        // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist7:"<<(ctx->txn_!=nullptr);
        plan->Validate(ctx);
        // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist3:"<<(ctx->txn_!=nullptr);
        // LOG_DEBUG() << "Command Type" <<plan->CommandType(); // de
        if (plan->CommandType() != parser::CmdType::QUERY && plan->CommandType() != parser::CmdType::MAINTENANCE
                && plan->CommandType() != parser::CmdType::PROFILE) {
            ctx->result_info_ = std::make_unique<ResultInfo>();
            ctx->result_ = std::make_unique<lgraph::Result>();
            std::string header, data;
            if (plan->CommandType() == parser::CmdType::EXPLAIN) {
                header = "@plan";
                data = plan->DumpPlan(0, false);
            } else if (plan->CommandType() == parser::CmdType::PROFILE){
                // header = "@profile";
                // TODO : Profile
                // data = plan->DumpGraph();
            }
            else if(visitor.CommandType() == parser::CmdType::OPTIMIZE) {
                LOG_DEBUG()<<"optimize start";
                const std::vector<cypher::PatternGraph>& pattern_graphs=plan->GetPatternGraphs();
                ReverseRewriteVisitor new_visitor(ctx,oc_cypher,pattern_graphs);
                LOG_DEBUG()<<"optimize end";
                if(is_with_new_txn){
                    ctx->result_info_ = std::make_unique<ResultInfo>();
                    ctx->result_ = std::make_unique<lgraph::Result>();

                    ctx->result_->ResetHeader({{"@opt_query", lgraph_api::LGraphType::STRING}});
                    auto r = ctx->result_->MutableRecord();
                    r->Insert("@opt_query", lgraph::FieldData(new_visitor.GetOptQuery()));
                }
                return new_visitor.GetOptQuery();
            }
            ctx->result_->ResetHeader({{header, lgraph_api::LGraphType::STRING}});
            auto r = ctx->result_->MutableRecord();
            r->Insert(header, lgraph::FieldData(data));
            if (ctx->bolt_conn_) {
                auto session = (bolt::BoltSession *)ctx->bolt_conn_->GetContext();
                while (!session->streaming_msg) {
                    session->streaming_msg = session->msgs.Pop(std::chrono::milliseconds(100));
                    if (ctx->bolt_conn_->has_closed()) {
                        LOG_INFO() << "The bolt connection is closed, cancel the op execution.";
                        return std::string();
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
            return std::string();
        }
        LOG_DEBUG() << "Plan cache disabled.";
    }
    LOG_DEBUG() << plan->DumpPlan(0, false);
    LOG_DEBUG() << plan->DumpGraph();
    elapsed.t_compile = fma_common::GetTime() - t0;
    LOG_DEBUG()<<"EvalCypherWithoutNewTxn2 txn exist4:"<<(ctx->txn_!=nullptr);
    if(!is_with_new_txn)
        plan->ExecuteWithoutNewTxn(ctx);
    else{
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
    }
    // if(!plan->ReadOnly())UpdateView(ctx);
    elapsed.t_total = fma_common::GetTime() - t0;
    elapsed.t_exec = elapsed.t_total - elapsed.t_compile;
    if (plan->CommandType() == parser::CmdType::PROFILE){
        size_t db_hit=plan->GetDBHit();
        size_t result_num=plan->Root()->stats.profileRecordCount;
        ctx->result_info_ = std::make_unique<ResultInfo>();
        ctx->result_ = std::make_unique<lgraph::Result>();

        ctx->result_->ResetHeader({{"db_hit", lgraph_api::LGraphType::STRING},{"result_num", lgraph_api::LGraphType::STRING}});
        auto r = ctx->result_->MutableRecord();
        r->Insert("db_hit", lgraph::FieldData(std::to_string(db_hit)));
        r->Insert("result_num", lgraph::FieldData(std::to_string(result_num)));
    }
    return std::string();

}
const std::string Scheduler::EvalCypherWithoutNewTxn(RTContext *ctx, const std::string &script, ElapsedTime &elapsed) {
    return EvalCypher(ctx,script,elapsed,false);
    // LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist:"<<(ctx->txn_!=nullptr);
    // using namespace parser;
    // using namespace antlr4;
    // // LOG_DEBUG()<<"tugraph dir: "<< ctx->galaxy_->GetConfig().dir;
    // auto t0 = fma_common::GetTime();
    // // <script, execution plan>
    // thread_local LRUCacheThreadUnsafe<std::string, std::shared_ptr<ExecutionPlan>> tls_plan_cache;
    // std::shared_ptr<ExecutionPlan> plan;
    // if (!tls_plan_cache.Get(script, plan)) {
    //     // auto rewrite_query=RewriteCypherUseViews(ctx,script);
    //     ANTLRInputStream input(script);
    //     LcypherLexer lexer(&input);
    //     CommonTokenStream tokens(&lexer);
    //     LOG_DEBUG() <<"parser s"<<std::endl; // de
    //     LcypherParser parser(&tokens);
    //     /* We can set ErrorHandler here.
    //      * setErrorHandler(std::make_shared<BailErrorStrategy>());
    //      * add customized ErrorListener  */
    //     LOG_DEBUG() <<"parser e"<<std::endl; // de
    //     parser.addErrorListener(&CypherErrorListener::INSTANCE);
    //     LOG_DEBUG() <<"visitor s"<<std::endl; // de
    //     auto oc_cypher=parser.oC_Cypher();
    //     CypherBaseVisitor visitor(ctx, oc_cypher);
    //     LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist2:"<<(ctx->txn_!=nullptr);
    //     LOG_DEBUG() <<"visitor e"<<std::endl; // de
    //     LOG_DEBUG() << "-----CLAUSE TO STRING-----";
    //     for (const auto &sql_query : visitor.GetQuery()) {
    //         LOG_DEBUG() << sql_query.ToString();
    //     }
    //     plan = std::make_shared<ExecutionPlan>();
    //     if(visitor.CommandType()==parser::CmdType::MAINTENANCE)
    //         plan.get()->SetMaintenance(true);
    //     else if(visitor.CommandType()==parser::CmdType::OPTIMIZE)
    //         plan.get()->SetOptimize(true);
    //     LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist6:"<<(ctx->txn_!=nullptr);
    //     plan->Build(visitor.GetQuery(), visitor.CommandType(), ctx);
    //     LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist7:"<<(ctx->txn_!=nullptr);
    //     plan->Validate(ctx);
    //     LOG_DEBUG()<<"EvalCypherWithoutNewTxn txn exist3:"<<(ctx->txn_!=nullptr);
    //     LOG_DEBUG() << "Command Type" <<plan->CommandType(); // de
    //     if (plan->CommandType() != parser::CmdType::QUERY) {
    //         ctx->result_info_ = std::make_unique<ResultInfo>();
    //         ctx->result_ = std::make_unique<lgraph::Result>();
    //         std::string header, data;
    //         if (plan->CommandType() == parser::CmdType::EXPLAIN) {
    //             header = "@plan";
    //             data = plan->DumpPlan(0, false);
    //         } else if (plan->CommandType() == parser::CmdType::PROFILE){
    //             header = "@profile";
    //             data = plan->DumpGraph();
    //         }
    //         else if(visitor.CommandType() == parser::CmdType::OPTIMIZE) {
    //             LOG_DEBUG()<<"optimize start";
    //             const std::vector<cypher::PatternGraph>& pattern_graphs=plan->GetPatternGraphs();
    //             ParseTreeToCypherVisitor new_visitor(ctx,oc_cypher,pattern_graphs);
    //             LOG_DEBUG()<<"optimize end";
    //             // ctx->result_info_ = std::make_unique<ResultInfo>();
    //             // ctx->result_ = std::make_unique<lgraph::Result>();

    //             // ctx->result_->ResetHeader({{"@opt_query", lgraph_api::LGraphType::STRING}});
    //             // auto r = ctx->result_->MutableRecord();
    //             // r->Insert("@opt_query", lgraph::FieldData(new_visitor.GetOptQuery()));
    //             return new_visitor.GetOptQuery();
    //         }
    //         else if(visitor.CommandType()==parser::CmdType::VIEW){ //创建视图
    //             LOG_DEBUG() << "s";
    //             const std::vector<cypher::PatternGraph>& pattern_graphs=plan->GetPatternGraphs();
    //             LOG_DEBUG() << "new visitor s";
    //             RewriteViews new_visitor(ctx,oc_cypher,pattern_graphs);
    //             LOG_DEBUG() << "new visitor e";
    //             auto view_name=new_visitor.GetViewName();
    //             auto constraints=new_visitor.GetConstraints();
    //             auto new_query=new_visitor.GetRewriteQuery();
    //             LOG_DEBUG() << "hhh";
    //             // std::string create_query="CALL db.createEdgeLabel('"+view_name+"', '[[\""+constraints.first+"\",\""+constraints.second+"\"]]','is_view',bool,true)";
    //             std::string create_query="CALL db.createEdgeLabel('"+view_name+"', '[]')";
    //             std::cout<<"create query: "<<create_query<<std::endl;
    //             std::cout<<"new query: "<<new_query<<std::endl;
    //             elapsed.t_compile = fma_common::GetTime() - t0;
    //             ElapsedTime temp;
    //             EvalCypherWithoutNewTxn(ctx,create_query,temp);
    //             // 将信息保存到json文件中
    //             auto parent_dir=ctx->galaxy_->GetConfig().dir;
    //             if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
    //             std::string file_path=parent_dir+"/view/"+ctx->graph_+".json";
    //             // std::string file_path="/data/view/"+ctx->graph_+".json";
    //             AddElement(file_path,view_name,constraints.first,constraints.second,new_query);

    //             EvalCypherWithoutNewTxn(ctx,new_query,temp);
    //             ctx->result_info_ = std::make_unique<ResultInfo>();
    //             ctx->result_ = std::make_unique<lgraph::Result>();

    //             ctx->result_->ResetHeader({{"view_name", lgraph_api::LGraphType::STRING},{"start_node_label", lgraph_api::LGraphType::STRING},{"end_node_label", lgraph_api::LGraphType::STRING}});
    //             auto r = ctx->result_->MutableRecord();
    //             r->Insert("view_name", lgraph::FieldData(view_name));
    //             r->Insert("start_node_label", lgraph::FieldData(constraints.first));
    //             r->Insert("end_node_label", lgraph::FieldData(constraints.second));
                
                
    //             elapsed.t_total = fma_common::GetTime() - t0;
    //             LOG_DEBUG() << "end";
    //             return std::string();
    //         }
    //         ctx->result_->ResetHeader({{header, lgraph_api::LGraphType::STRING}});
    //         auto r = ctx->result_->MutableRecord();
    //         r->Insert(header, lgraph::FieldData(data));
    //         if (ctx->bolt_conn_) {
    //             auto session = (bolt::BoltSession *)ctx->bolt_conn_->GetContext();
    //             while (!session->streaming_msg) {
    //                 session->streaming_msg = session->msgs.Pop(std::chrono::milliseconds(100));
    //                 if (ctx->bolt_conn_->has_closed()) {
    //                     LOG_INFO() << "The bolt connection is closed, cancel the op execution.";
    //                     return std::string();
    //                 }
    //             }
    //             std::unordered_map<std::string, std::any> meta;
    //             meta["fields"] = ctx->result_->BoltHeader();
    //             bolt::PackStream ps;
    //             ps.AppendSuccess(meta);
    //             if (session->streaming_msg.value().type == bolt::BoltMsg::PullN) {
    //                 ps.AppendRecords(ctx->result_->BoltRecords());
    //             } else if (session->streaming_msg.value().type == bolt::BoltMsg::DiscardN) {
    //                 // ...
    //             }
    //             ps.AppendSuccess();
    //             ctx->bolt_conn_->PostResponse(std::move(ps.MutableBuffer()));
    //         }
    //         return std::string();
    //     }
    //     LOG_DEBUG() << "Plan cache disabled.";
    // }
    // LOG_DEBUG() << plan->DumpPlan(0, false);
    // LOG_DEBUG() << plan->DumpGraph();
    // elapsed.t_compile = fma_common::GetTime() - t0;
    // LOG_DEBUG()<<"EvalCypherWithoutNewTxn2 txn exist4:"<<(ctx->txn_!=nullptr);
    // plan->ExecuteWithoutNewTxn(ctx);
    // // if (!plan->ReadOnly() && ctx->optimistic_) {
    // //     while (1) {
    // //         try {
    // //             plan->Execute(ctx);
    // //             break;
    // //         } catch (lgraph::TxnCommitException &e) {
    // //             LOG_DEBUG() << e.what();
    // //         }
    // //     }
    // // } else {
    // //     plan->Execute(ctx);
    // // }
    // // if(!plan->ReadOnly())UpdateView(ctx);
    // elapsed.t_total = fma_common::GetTime() - t0;
    // elapsed.t_exec = elapsed.t_total - elapsed.t_compile;
    // return std::string();
}

void Scheduler::EvalCypher(RTContext *ctx, const std::string &script, ElapsedTime &elapsed) {
    EvalCypher(ctx,script,elapsed,true);
    // using namespace parser;
    // using namespace antlr4;
    // // LOG_DEBUG()<<"tugraph dir: "<< ctx->galaxy_->GetConfig().dir;
    // auto t0 = fma_common::GetTime();
    // // <script, execution plan>
    // thread_local LRUCacheThreadUnsafe<std::string, std::shared_ptr<ExecutionPlan>> tls_plan_cache;
    // std::shared_ptr<ExecutionPlan> plan;
    // if (!tls_plan_cache.Get(script, plan)) {
    //     // auto rewrite_query=RewriteCypherUseViews(ctx,script);
    //     ANTLRInputStream input(script);
    //     LcypherLexer lexer(&input);
    //     CommonTokenStream tokens(&lexer);
    //     LOG_DEBUG() <<"parser s"<<std::endl; // de
    //     LcypherParser parser(&tokens);
    //     /* We can set ErrorHandler here.
    //      * setErrorHandler(std::make_shared<BailErrorStrategy>());
    //      * add customized ErrorListener  */
    //     LOG_DEBUG() <<"parser e"<<std::endl; // de
    //     parser.addErrorListener(&CypherErrorListener::INSTANCE);
    //     LOG_DEBUG() <<"visitor s"<<std::endl; // de
    //     auto oc_cypher=parser.oC_Cypher();
    //     CypherBaseVisitor visitor(ctx, oc_cypher);
    //     LOG_DEBUG() <<"visitor e"<<std::endl; // de
    //     LOG_DEBUG() << "-----CLAUSE TO STRING-----";
    //     for (const auto &sql_query : visitor.GetQuery()) {
    //         LOG_DEBUG() << sql_query.ToString();
    //     }
    //     plan = std::make_shared<ExecutionPlan>();
    //     if(visitor.CommandType()==parser::CmdType::MAINTENANCE)
    //         plan.get()->SetMaintenance(true);
    //     else if(visitor.CommandType()==parser::CmdType::OPTIMIZE)
    //         plan.get()->SetOptimize(true);
    //     plan->Build(visitor.GetQuery(), visitor.CommandType(), ctx);
    //     plan->Validate(ctx);
    //     LOG_DEBUG() << "Command Type" <<plan->CommandType(); // de
    //     if (plan->CommandType() != parser::CmdType::QUERY) {
    //         ctx->result_info_ = std::make_unique<ResultInfo>();
    //         ctx->result_ = std::make_unique<lgraph::Result>();
    //         std::string header, data;
    //         if (plan->CommandType() == parser::CmdType::EXPLAIN) {
    //             header = "@plan";
    //             data = plan->DumpPlan(0, false);
    //         } else if (plan->CommandType() == parser::CmdType::PROFILE){
    //             header = "@profile";
    //             data = plan->DumpGraph();
    //         } else if(visitor.CommandType() == parser::CmdType::OPTIMIZE) {
    //             const std::vector<cypher::PatternGraph>& pattern_graphs=plan->GetPatternGraphs();
    //             ParseTreeToCypherVisitor new_visitor(ctx,oc_cypher,pattern_graphs);
    //             ctx->result_info_ = std::make_unique<ResultInfo>();
    //             ctx->result_ = std::make_unique<lgraph::Result>();

    //             ctx->result_->ResetHeader({{"@opt_query", lgraph_api::LGraphType::STRING}});
    //             auto r = ctx->result_->MutableRecord();
    //             r->Insert("@opt_query", lgraph::FieldData(new_visitor.GetOptQuery()));
    //             return;
    //         }
    //         else{ //创建视图
    //             LOG_DEBUG() << "s";
    //             const std::vector<cypher::PatternGraph>& pattern_graphs=plan->GetPatternGraphs();
    //             LOG_DEBUG() << "new visitor s";
    //             RewriteViews new_visitor(ctx,oc_cypher,pattern_graphs);
    //             LOG_DEBUG() << "new visitor e";
    //             auto view_name=new_visitor.GetViewName();
    //             auto constraints=new_visitor.GetConstraints();
    //             auto new_query=new_visitor.GetRewriteQuery();
    //             LOG_DEBUG() << "hhh";
    //             // std::string create_query="CALL db.createEdgeLabel('"+view_name+"', '[[\""+constraints.first+"\",\""+constraints.second+"\"]]','is_view',bool,true)";
    //             std::string create_query="CALL db.createEdgeLabel('"+view_name+"', '[]')";
    //             std::cout<<"create query: "<<create_query<<std::endl;
    //             std::cout<<"new query: "<<new_query<<std::endl;
    //             elapsed.t_compile = fma_common::GetTime() - t0;
    //             ElapsedTime temp;
    //             EvalCypher(ctx,create_query,temp);
    //             // 将信息保存到json文件中
    //             auto parent_dir=ctx->galaxy_->GetConfig().dir;
    //             if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
    //             std::string file_path=parent_dir+"/view/"+ctx->graph_+".json";
    //             // std::string file_path="/data/view/"+ctx->graph_+".json";
    //             AddElement(file_path,view_name,constraints.first,constraints.second,new_query);

    //             EvalCypher(ctx,new_query,temp);
    //             ctx->result_info_ = std::make_unique<ResultInfo>();
    //             ctx->result_ = std::make_unique<lgraph::Result>();

    //             ctx->result_->ResetHeader({{"view_name", lgraph_api::LGraphType::STRING},{"start_node_label", lgraph_api::LGraphType::STRING},{"end_node_label", lgraph_api::LGraphType::STRING}});
    //             auto r = ctx->result_->MutableRecord();
    //             r->Insert("view_name", lgraph::FieldData(view_name));
    //             r->Insert("start_node_label", lgraph::FieldData(constraints.first));
    //             r->Insert("end_node_label", lgraph::FieldData(constraints.second));
                
                
    //             elapsed.t_total = fma_common::GetTime() - t0;
    //             LOG_DEBUG() << "end";
    //             return;
    //         }
    //         ctx->result_->ResetHeader({{header, lgraph_api::LGraphType::STRING}});
    //         auto r = ctx->result_->MutableRecord();
    //         r->Insert(header, lgraph::FieldData(data));
    //         if (ctx->bolt_conn_) {
    //             auto session = (bolt::BoltSession *)ctx->bolt_conn_->GetContext();
    //             while (!session->streaming_msg) {
    //                 session->streaming_msg = session->msgs.Pop(std::chrono::milliseconds(100));
    //                 if (ctx->bolt_conn_->has_closed()) {
    //                     LOG_INFO() << "The bolt connection is closed, cancel the op execution.";
    //                     return;
    //                 }
    //             }
    //             std::unordered_map<std::string, std::any> meta;
    //             meta["fields"] = ctx->result_->BoltHeader();
    //             bolt::PackStream ps;
    //             ps.AppendSuccess(meta);
    //             if (session->streaming_msg.value().type == bolt::BoltMsg::PullN) {
    //                 ps.AppendRecords(ctx->result_->BoltRecords());
    //             } else if (session->streaming_msg.value().type == bolt::BoltMsg::DiscardN) {
    //                 // ...
    //             }
    //             ps.AppendSuccess();
    //             ctx->bolt_conn_->PostResponse(std::move(ps.MutableBuffer()));
    //         }
    //         return;
    //     }
    //     LOG_DEBUG() << "Plan cache disabled.";
    // }
    // LOG_DEBUG() << plan->DumpPlan(0, false);
    // LOG_DEBUG() << plan->DumpGraph();
    // elapsed.t_compile = fma_common::GetTime() - t0;
    // if (!plan->ReadOnly() && ctx->optimistic_) {
    //     while (1) {
    //         try {
    //             plan->Execute(ctx);
    //             break;
    //         } catch (lgraph::TxnCommitException &e) {
    //             LOG_DEBUG() << e.what();
    //         }
    //     }
    // } else {
    //     plan->Execute(ctx);
    // }
    // // if(!plan->ReadOnly())UpdateView(ctx);
    // elapsed.t_total = fma_common::GetTime() - t0;
    // elapsed.t_exec = elapsed.t_total - elapsed.t_compile;
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
