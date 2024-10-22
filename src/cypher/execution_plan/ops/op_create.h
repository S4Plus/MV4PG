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
// Created by wt on 18-7-27.
//
#pragma once

#include "parser/clause.h"
#include "parser/view_maintenance_visitor.h"
#include "parser/varlen_unfold_visitor.h"
#include "parser/generated/LcypherLexer.h"
#include "parser/generated/LcypherParser.h"
#include "cypher/execution_plan/ops/op.h"
#include "cypher/execution_plan/scheduler.h"
namespace cypher {

class OpCreate : public OpBase {
    const SymbolTable &sym_tab_;
    std::vector<parser::Clause::TYPE_CREATE> create_data_;
    PatternGraph *pattern_graph_ = nullptr;
    bool standalone_ = false;
    bool summary_ = false;

    void _SetViewInfs(std::string view_path){
        #include <fstream>
        #include <nlohmann/json.hpp>
        // #include "execution_plan/runtime_context.h"
        // #include "db/galaxy.h"
        // auto parent_dir=ctx->galaxy_->GetConfig().dir;
        // if(parent_dir.end()[-1]=='/')parent_dir.pop_back();
        // std::string file_path="/data/view/"+ctx->graph_+".json";
        std::ifstream ifs(view_path);
        nlohmann::json j;
        try {
            ifs >> j;
        } catch (nlohmann::json::parse_error& e) {
            j = nlohmann::json::array();
        }
        ifs.close();
        if(j.size()>0){
            for(auto element:j.at(0).items()){
                view_names_.emplace(element.key());
                edge_maintenances_.emplace(element.value().at("create_edge"));
            }
        }
        // for (auto& element : j) {
        //     view_names_.emplace(element["view_name"]);
        //     view_queries_.emplace(element["query"]);
        // }
    }

    void ViewMaintenanceCreateEdge(RTContext *ctx,lgraph::EdgeUid edge_uid) {
        auto txn=ctx->txn_->GetTxn().get();
        auto label=txn->GetEdgeLabel(edge_uid.lid);
        // auto label=txn->GetEdgeLabel(edge_uid);
        if(view_names_.find(label)!=view_names_.end())return;

        auto src_label=txn->GetVertexLabel(edge_uid.src);
        auto src_primary_field=txn->GetVertexPrimaryField(src_label);

        std::cout<<"View maintenance2"<<std::endl;
        auto src_primary_value=(ctx->txn_->GetTxn().get()->GetVertexField(edge_uid.src,src_primary_field));
        bool src_is_string=src_primary_value.IsString();

        auto dst_label=txn->GetVertexLabel(edge_uid.dst);
        auto dst_primary_field=txn->GetVertexPrimaryField(dst_label);

        std::cout<<"View maintenance3"<<std::endl;
        auto dst_primary_value=(ctx->txn_->GetTxn().get()->GetVertexField(edge_uid.dst,dst_primary_field));
        bool dst_is_string=dst_primary_value.IsString();
        std::tuple<std::string,std::string,std::string,bool> src_info(src_label,src_primary_field,src_primary_value.ToString(),src_is_string);
        std::tuple<std::string,std::string,std::string,bool> dst_info(dst_label,dst_primary_field,dst_primary_value.ToString(),dst_is_string);  
        using namespace parser;
        using namespace antlr4;
        for(auto view_query:edge_maintenances_){
            auto temp1=Replace(view_query,"\\$SL",src_label);
            auto temp2=Replace(temp1,"\\$SK",src_primary_field);
            std::string temp3;
            if(src_is_string)
                temp3=Replace(temp2,"\\$SV","'"+src_primary_value.ToString()+"'");
            else
                temp3=Replace(temp2,"\\$SV",src_primary_value.ToString());
            auto temp4=Replace(temp3,"\\$DL",dst_label);
            auto temp5=Replace(temp4,"\\$DK",dst_primary_field);
            std::string temp6;
            if(dst_is_string)
                temp6=Replace(temp5,"\\$DV","'"+dst_primary_value.ToString()+"'");
            else
                temp6=Replace(temp5,"\\$DV",dst_primary_value.ToString());
            auto result=Replace(temp6,"\\$RID","'"+edge_uid.ToString()+"'");
            std::cout<<"View maintenance5: "<<result<<std::endl;
            cypher::ElapsedTime temp;
            Scheduler scheduler;
            // scheduler.Eval(ctx,lgraph_api::GraphQueryType::CYPHER,"match (n) return count(n)",temp);
            LOG_DEBUG()<<"in create op txn exist:"<<(ctx->txn_!=nullptr);
            scheduler.EvalCypherWithoutNewTxn(ctx,result,temp);
            std::cout<<"View maintenance6: "<<std::endl;
            // ANTLRInputStream input(view_query);
            // LcypherLexer lexer(&input);
            // CommonTokenStream tokens(&lexer);
            // // std::cout <<"parser s1"<<std::endl; // de
            // LcypherParser parser(&tokens);
            // VarlenUnfoldVisitor visitor(parser.oC_Cypher());
            // auto unfold_queries=visitor.GetRewriteQueries();
            // for(auto unfold_query:unfold_queries){
                // schema重写优化
                // cypher::ElapsedTime temp;
                // Scheduler scheduler;
                // auto new_unfold_query=scheduler.EvalCypherWithoutNewTxn(ctx,"optimize "+unfold_query,temp);
                // LOG_DEBUG()<<"unfold query:"<<unfold_query;
                // LOG_DEBUG()<<"new unfold query:"<<new_unfold_query;
                //获得视图更新语句
                // ANTLRInputStream input(view_query);
                // LcypherLexer lexer(&input);
                // CommonTokenStream tokens(&lexer);
                // // std::cout <<"parser s1"<<std::endl; // de
                // LcypherParser parser(&tokens);
                // ViewMaintenance visitor(parser.oC_Cypher(),label,edge_uid.eid,src_info,dst_info,true);
                // std::cout<<"View maintenance4: "<<std::endl;
                // // std::string rewrite_query=visitor.GetRewriteQueries();
                // // std::cout<<"View maintenance5: "<<rewrite_query<<std::endl;
                // // cypher::ElapsedTime temp;
                // // Scheduler scheduler;
                // // // scheduler.Eval(ctx,lgraph_api::GraphQueryType::CYPHER,"match (n) return count(n)",temp);
                // // LOG_DEBUG()<<"in create op txn exist:"<<(ctx->txn_!=nullptr);
                // // scheduler.EvalCypherWithoutNewTxn(ctx,rewrite_query,temp);
                // // std::cout<<"View maintenance6: "<<std::endl; 
                // std::vector<std::string> queries=visitor.GetRewriteQueries();
                // for(auto query:queries){
                //     std::cout<<"View maintenance5: "<<query<<std::endl;
                //     cypher::ElapsedTime temp;
                //     Scheduler scheduler;
                //     // scheduler.Eval(ctx,lgraph_api::GraphQueryType::CYPHER,"match (n) return count(n)",temp);
                //     LOG_DEBUG()<<"in create op txn exist:"<<(ctx->txn_!=nullptr);
                //     scheduler.EvalCypherWithoutNewTxn(ctx,query,temp);
                //     std::cout<<"View maintenance6: "<<std::endl; 
                // }
            // }
        }
    }

    void ExtractProperties(RTContext *ctx, const parser::TUP_PROPERTIES &properties,
                           VEC_STR &fields, std::vector<lgraph::FieldData> &values) {
        using namespace parser;
        Expression map_literal = std::get<0>(properties);
        CYPHER_THROW_ASSERT(map_literal.type == Expression::NA ||
                            map_literal.type == Expression::MAP);
        if (map_literal.type != Expression::MAP) return;
        for (auto &prop : map_literal.Map()) {
            fields.emplace_back(prop.first);
            Expression p;
            if (prop.second.type == Expression::LIST) {
                p = prop.second.List().at(0);
            } else if (prop.second.type == Expression::MAP) {
                CYPHER_TODO();
            } else {
                p = prop.second;
            }
            ArithExprNode ae(p, *record->symbol_table);
            auto value = ae.Evaluate(ctx, *record);
            CYPHER_THROW_ASSERT(value.IsScalar());
            values.emplace_back(value.constant.scalar);
        }
    }


    void CreateVertex(RTContext *ctx, const parser::TUP_NODE_PATTERN &node_pattern) {
        using namespace parser;
        auto &node_variable = std::get<0>(node_pattern);
        auto &node_labels = std::get<1>(node_pattern);
        auto &properties = std::get<2>(node_pattern);
        if (node_labels.empty())
            throw lgraph::EvaluationException("vertex label missing in create.");
        // cypher::ElapsedTime temp;
        // Scheduler scheduler;
        // // scheduler.Eval(ctx,lgraph_api::GraphQueryType::CYPHER,"match (n) return count(n)",temp);
        // scheduler.EvalCypher(ctx,"match (n) return count(n)",temp,false);
        const std::string &label = *node_labels.begin();
        VEC_STR fields;
        std::vector<lgraph::FieldData> values;
        ExtractProperties(ctx, properties, fields, values);
        auto vid = ctx->txn_->AddVertex(label, fields, values);
        ctx->result_info_->statistics.vertices_created++;
        // update pattern graph
        // add isolated node
        if (!node_variable.empty()) {
            /* There could be multiple match,
             * e.g. MATCH (a:Film) CREATE (b)  */
            auto node = &pattern_graph_->GetNode(node_variable);
            if (node->Empty()) CYPHER_TODO();
            node->Visited() = true;
            node->PushVid(vid);
            // fill the record
            if (!summary_) {
                auto it = sym_tab_.symbols.find(node_variable);
                CYPHER_THROW_ASSERT(it != sym_tab_.symbols.end());
                record->values[it->second.id].type = Entry::NODE;
                record->values[it->second.id].node = node;
            }
        }
    }

    void CreateEdge(RTContext *ctx, const parser::TUP_NODE_PATTERN &lhs_patt,
                    const parser::TUP_NODE_PATTERN &rhs_patt,
                    const parser::TUP_RELATIONSHIP_PATTERN &relp_pattern) {
        using namespace parser;
        auto src_node_var = std::get<0>(lhs_patt);
        auto dst_node_var = std::get<0>(rhs_patt);
        auto direction = std::get<0>(relp_pattern);
        auto &relationship_detail = std::get<1>(relp_pattern);
        if (direction == LinkDirection::RIGHT_TO_LEFT) {
            std::swap(src_node_var, dst_node_var);
        } else if (direction != LinkDirection::LEFT_TO_RIGHT) {
            throw lgraph::CypherException("Only directed relationships are supported in CREATE");
        }
        auto &edge_variable = std::get<0>(relationship_detail);
        auto &relationship_types = std::get<1>(relationship_detail);
        auto &properties = std::get<3>(relationship_detail);
        // add edge
        if (relationship_types.empty())
            throw lgraph::CypherException("Edge label missing in create");
        // TODO(anyone) use children's record instead?
        auto &src_node = pattern_graph_->GetNode(src_node_var);
        auto &dst_node = pattern_graph_->GetNode(dst_node_var);
        auto &label = relationship_types[0];
        if (src_node.PullVid() < 0 || dst_node.PullVid() < 0) {
            throw lgraph::CypherException(
                FMA_FMT("Invalid vertex when create edge [{}]: {}", edge_variable,
                        src_node.PullVid() < 0 ? src_node_var : dst_node_var));
        }
        VEC_STR fields;
        std::vector<lgraph::FieldData> values;
        ExtractProperties(ctx, properties, fields, values);
        auto euid =
            ctx->txn_->AddEdge(src_node.PullVid(), dst_node.PullVid(), label, fields, values);
        ctx->result_info_->statistics.edges_created++;
        if(is_maintenance_view_){
            edge_created_++;
            view_label=label;
        }
        // LOG_DEBUG()<<"create Edge:"<<euid.ToString();
        if (!edge_variable.empty()) {
            /* There could be multiple match,
             * e.g. MATCH (a:Film),(b:City) CREATE (a)-[r:BORN_IN]->(b)  */
            auto relp = &pattern_graph_->GetRelationship(edge_variable);
            if (relp->Empty()) CYPHER_TODO();
            relp->ItRef()->Initialize(ctx->txn_->GetTxn().get(), euid);
            // fill the record
            if (!summary_) {
                auto it = sym_tab_.symbols.find(edge_variable);
                CYPHER_THROW_ASSERT(it != sym_tab_.symbols.end());
                record->values[it->second.id].type = Entry::RELATIONSHIP;
                record->values[it->second.id].relationship = relp;
            }
        }
        if(!is_create_view_&&!is_maintenance_view_)
            ViewMaintenanceCreateEdge(ctx,euid);
    }

    void CreateVE(RTContext *ctx) {
        // LOG_DEBUG()<<"Create VE start";
        for (auto &pattern : create_data_) {
            for (auto &pattern_part : pattern) {
                auto pp_variable = std::get<0>(pattern_part);
                if (!pp_variable.empty()) CYPHER_TODO();
                auto pattern_element = std::get<1>(pattern_part);
                auto lhs_patt = std::get<0>(pattern_element);
                auto pattern_element_chains = std::get<1>(pattern_element);
                if (pattern_element_chains.empty()) {  // create vertex
                    CreateVertex(ctx, lhs_patt);
                } else {  // create chains
                    for (auto &chain : pattern_element_chains) {
                        auto &rhs_patt = std::get<1>(chain);
                        auto &relp_pattern = std::get<0>(chain);
                        auto &lhs_node = pattern_graph_->GetNode(std::get<0>(lhs_patt));
                        auto &rhs_node = pattern_graph_->GetNode(std::get<0>(rhs_patt));
#ifndef NDEBUG
                        LOG_DEBUG()<<"create vertex 1";
#endif
                        if (lhs_node.derivation_ == Node::CREATED && !lhs_node.Visited()) {
                            CreateVertex(ctx, lhs_patt);
                        }
#ifndef NDEBUG
                        LOG_DEBUG()<<"create vertex 2";
#endif
                        if (rhs_node.derivation_ == Node::CREATED && !rhs_node.Visited()) {
                            CreateVertex(ctx, rhs_patt);
                        }
#ifndef NDEBUG
                        LOG_DEBUG()<<"create vertex 3";
#endif
                        CreateEdge(ctx, lhs_patt, rhs_patt, relp_pattern);
#ifndef NDEBUG
                        LOG_DEBUG()<<"create vertex 4";
#endif
                        lhs_patt = rhs_patt;
                    }
                }
            }  // for pattern_part
        }
        if(is_maintenance_view_){
            MaintenanceViewStatistics(view_path_,view_label,edge_created_,false);
            edge_created_=0;
        }
        ctx->txn_->GetTxn()->RefreshIterators();
        // LOG_DEBUG()<<"Create VE end";
    }

    void ResultSummary(RTContext *ctx) {
        if (summary_) {
            std::string summary;
            summary.append("created ")
                .append(std::to_string(ctx->result_info_->statistics.vertices_created))
                .append(" vertices, created ")
                .append(std::to_string(ctx->result_info_->statistics.edges_created))
                .append(" edges.");
            // CYPHER_THROW_ASSERT(ctx->result_->Header().size() == 1);
            CYPHER_THROW_ASSERT(record);
            record->values.clear();
            record->AddConstant(lgraph::FieldData(summary));
        } else {
            /* There should be a "Project" operation on top of this
             * "Create", so leave result set to it. */
        }
    }

 public:
    std::string view_path_;
    std::set<std::string> view_names_;
    std::set<std::string> edge_maintenances_;
    bool is_create_view_ = false;
    bool is_maintenance_view_ =false;
    int edge_created_=0;
    std::string view_label="";

    OpCreate(const parser::QueryPart *stmt, PatternGraph *pattern_graph)
        : OpBase(OpType::CREATE, "Create"),
          sym_tab_(pattern_graph->symbol_table),
          pattern_graph_(pattern_graph) {
        for (auto &node : pattern_graph_->GetNodes()) {
            if (node.derivation_ == Node::CREATED) modifies.emplace_back(node.Alias());
            for (auto rr : node.RhsRelps()) {
                auto &r = pattern_graph_->GetRelationship(rr);
                if (r.derivation_ == Relationship::CREATED) modifies.emplace_back(r.Alias());
            }
        }
        for (auto c : stmt->create_clause) {
            create_data_.emplace_back(*c);
        }
    }

    OpCreate(const parser::QueryPart *stmt, PatternGraph *pattern_graph, std::string view_path,bool is_create_view,bool is_maintenance_view)
        : OpBase(OpType::CREATE, "Create"),
          sym_tab_(pattern_graph->symbol_table),
          pattern_graph_(pattern_graph),
          view_path_(view_path),
          is_create_view_(is_create_view),
          is_maintenance_view_(is_maintenance_view) {
        _SetViewInfs(view_path_);
        for (auto &node : pattern_graph_->GetNodes()) {
            if (node.derivation_ == Node::CREATED) modifies.emplace_back(node.Alias());
            for (auto rr : node.RhsRelps()) {
                auto &r = pattern_graph_->GetRelationship(rr);
                if (r.derivation_ == Relationship::CREATED) modifies.emplace_back(r.Alias());
            }
        }
        for (auto c : stmt->create_clause) {
            create_data_.emplace_back(*c);
        }
    }

    OpResult Initialize(RTContext *ctx) override {
        CYPHER_THROW_ASSERT(parent && children.size() < 2);
        summary_ = !parent->parent;
        standalone_ = children.empty();
        for (auto child : children) {
            child->Initialize(ctx);
        }
        record = children.empty() ?
            std::make_shared<Record>(sym_tab_.symbols.size(), &sym_tab_, ctx->param_tab_)
                                  : children[0]->record;
        return OP_OK;
    }

    OpResult RealConsume(RTContext *ctx) override {
        if (state == StreamDepleted) return OP_DEPLETED;
        if (standalone_) {
            CreateVE(ctx);
            ResultSummary(ctx);
            state = StreamDepleted;
            return OP_OK;
        } else {
            if (children.size() > 1) CYPHER_TODO();
            auto child = children[0];
            if (summary_) {
#ifndef NDEBUG
                LOG_DEBUG()<<"create consume 1";
#endif
                while (child->Consume(ctx) == OP_OK) CreateVE(ctx);
                ResultSummary(ctx);
                state = StreamDepleted;
                return OP_OK;
            } else {
                if (child->Consume(ctx) == OP_OK) {
                    CreateVE(ctx);
                    return OP_OK;
                } else {
                    return OP_DEPLETED;
                }
            }
        }
    }

    OpResult ResetImpl(bool complete) override {
        state = StreamUnInitialized;
        return OP_OK;
    }

    std::string ToString() const override {
        std::string str(name);
        str.append(" [");
        for (auto &m : modifies) str.append(m).append(",");
        if (!modifies.empty()) str.pop_back();
        str.append("]");
        return str;
    }

    const SymbolTable& SymTab() const { return sym_tab_; }

    PatternGraph* GetPatternGraph() const { return pattern_graph_; }

    CYPHER_DEFINE_VISITABLE()

    CYPHER_DEFINE_CONST_VISITABLE()
};
}  // namespace cypher
