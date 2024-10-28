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
// Created by dcy on 19-8-22.
//
#pragma once

#include "cypher/execution_plan/ops/op.h"
#include "cypher/execution_plan/scheduler.h"
#include "parser/clause.h"
#include "parser/view_maintenance_visitor.h"
#include "parser/varlen_unfold_visitor.h"
#include "parser/generated/LcypherLexer.h"
#include "parser/generated/LcypherParser.h"

namespace cypher {

class OpDelete : public OpBase {
    parser::Clause::TYPE_DELETE delete_data_;
    PatternGraph *pattern_graph_ = nullptr;
    std::vector<lgraph::VertexId> vertices_to_delete_;
    std::vector<lgraph::EdgeUid> edges_to_delete_;
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
                vertex_maintenances_.emplace(element.value().at("delete_vertex"));
                edge_maintenances_.emplace(element.value().at("delete_edge"));
            }
        }
    }

    void ViewMaintenanceDeleteVertex(RTContext *ctx,size_t vid) {
        std::cout<<"View maintenance1"<<std::endl;
        auto txn=ctx->txn_->GetTxn().get();
        auto label=txn->GetVertexLabel(vid);
        auto primary_field=txn->GetVertexPrimaryField(label);
        //////////////加入视图路径获取，判断label是否在视图中，在则不维护
        if(view_names_.find(label)!=view_names_.end())return;
        //////////////
        std::cout<<"View maintenance2"<<std::endl;
        auto primary_value=(ctx->txn_->GetTxn().get()->GetVertexField(vid,primary_field));
        bool is_string=primary_value.IsString();
        using namespace parser;
        using namespace antlr4;
        for(auto view_query:vertex_maintenances_){
            auto temp1=Replace(view_query,"\\$L",label);
            auto temp2=Replace(temp1,"\\$K",primary_field);
            std::string result;
            if(!is_string)
                result=Replace(temp2,"\\$V",primary_value.ToString());
            else
                result=Replace(temp2,"\\$V","'"+primary_value.ToString()+"'");
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
            // for(auto unfold_auery:unfold_queries){
                // schema重写优化
                // cypher::ElapsedTime temp;
                // Scheduler scheduler;
                // auto new_unfold_query=scheduler.EvalCypherWithoutNewTxn(ctx,"optimize "+unfold_auery,temp);
                //获得视图更新语句
                // ANTLRInputStream input(view_query);
                // LcypherLexer lexer(&input);
                // CommonTokenStream tokens(&lexer);
                // // std::cout <<"parser s1"<<std::endl; // de
                // LcypherParser parser(&tokens);
                // ViewMaintenance visitor(parser.oC_Cypher(),label,primary_field,primary_value.ToString(),is_string,false);
                // std::cout<<"View maintenance4: "<<std::endl;
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
        std::cout<<"View maintenance7: "<<std::endl;
    }

    void ViewMaintenanceDeleteEdge(RTContext *ctx,lgraph::EdgeUid edge_uid) {
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

        std::cout<<"View maintenance2"<<std::endl;
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
            // for(auto unfold_auery:unfold_queries){
                // schema重写优化
                // cypher::ElapsedTime temp;
                // Scheduler scheduler;
                // auto new_unfold_query=scheduler.EvalCypherWithoutNewTxn(ctx,"optimize "+unfold_auery,temp);
                // //获得视图更新语句
                // ANTLRInputStream input(view_query);
                // LcypherLexer lexer(&input);
                // CommonTokenStream tokens(&lexer);
                // // std::cout <<"parser s1"<<std::endl; // de
                // LcypherParser parser(&tokens);
                // ViewMaintenance visitor(parser.oC_Cypher(),label,edge_uid.eid,src_info,dst_info,false);
                // std::cout<<"View maintenance4: "<<std::endl;
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

    void CollectVEToDelete() {
        for (auto &dd : delete_data_) {
            auto it = record->symbol_table->symbols.find(dd.ToString());
            CYPHER_THROW_ASSERT(it != record->symbol_table->symbols.end());
            if (it->second.type == SymbolNode::NODE) {
                auto node = record->values[it->second.id].node;
                if (node && node->PullVid() >= 0) {
                    vertices_to_delete_.emplace_back(node->PullVid());
                    /* TODO(anyone) if buffer size overflow, do this task with
                     * optimized op. dump ids to delete into tmp file, note that
                     * manage file with object and delete file in destructor,
                     * make sure the tmp files removed after exception occurs.
                     */
                    if (vertices_to_delete_.size() > DELETE_BUFFER_SIZE) {
                        CYPHER_TODO();
                    }
                }
            } else if (it->second.type == SymbolNode::RELATIONSHIP) {
                auto relp = record->values[it->second.id].relationship;
                if (relp && relp->ItRef() && relp->ItRef()->IsValid()) {
                    edges_to_delete_.emplace_back(relp->ItRef()->GetUid());
                    if (edges_to_delete_.size() > DELETE_BUFFER_SIZE) {
                        CYPHER_TODO();
                    }
                }
            } else {
                CYPHER_TODO();
            }
        }
    }

    // void MaintenanceViewStatistics(std::string view_label,int count,bool is_delete){
    //     if(view_label=="")return;
    //     std::ifstream input_file(view_path_);
    //     json j;
    //     input_file >> j;

    //     int sign=is_delete?-1:1;
    //     j[view_label]["result_num"] = j[view_label]["result_num"]+count*sign;

    //     std::ofstream output_file(view_path_);
    //     output_file << std::setw(4) << j << std::endl;
    // }

    void DoDeleteVE(RTContext *ctx) {
        LOG_DEBUG() << "vertices & edges to delete:";
        for (auto &v : vertices_to_delete_) LOG_DEBUG() << "V[" << v << "]";
        for (auto &e : edges_to_delete_) LOG_DEBUG() << "E[" << _detail::EdgeUid2String(e) << "]";
        std::string view_label="";
        if(edges_to_delete_.size()>0){
            auto txn=ctx->txn_->GetTxn().get();
            view_label=txn->GetEdgeLabel(edges_to_delete_[0].lid);
        }
        for (auto &e : edges_to_delete_) {
            if(!is_view_)
                ViewMaintenanceDeleteEdge(ctx,e);
            if (ctx->txn_->GetTxn()->DeleteEdge(e)) {
                ctx->result_info_->statistics.edges_deleted++;
                if(is_view_)edge_deleted_++;
            }
        }
        if(is_view_){
            MaintenanceViewStatistics(view_path_,view_label,edge_deleted_,true);
            edge_deleted_=0;
        }
        for (auto &v : vertices_to_delete_) {
            size_t n_in, n_out;
            if(!is_view_)
                ViewMaintenanceDeleteVertex(ctx,v);
            // LOG_DEBUG() << "Delete vertex: " << v;
            // // lgraph_api::GraphDB db(ctx->ac_db_.get(), false);
            // LOG_DEBUG() << "get db : " << v;
            // // ctx->txn_ = std::make_unique<lgraph_api::Transaction>(db.CreateWriteTxn(ctx->optimistic_));
            // LOG_DEBUG() << "get db success: " << v;
            // auto txn=ctx->txn_->GetTxn();
            // LOG_DEBUG() << "get txn success: " << v;
            if (ctx->txn_->GetTxn()->DeleteVertex(v, &n_in, &n_out)) {
                LOG_DEBUG() << "Delete vertex end: " << v;
                ctx->result_info_->statistics.vertices_deleted++;
                ctx->result_info_->statistics.edges_deleted += n_in + n_out;
            }
        }
        /* NOTE:
         * lgraph::EdgeIterator will refresh itself after calling
         * Delete(), and point to the next element.
         * While lgraph::Transaction::DeleteEdge() will not refresh
         * the iterator, after calling this method, the edge iterator
         * just becomes invalid.  */
        ctx->txn_->GetTxn()->RefreshIterators();
    }

    void ResultSummary(RTContext *ctx) {
        if (summary_) {
            std::string summary;
            summary.append("deleted ")
                .append(std::to_string(ctx->result_info_->statistics.vertices_deleted))
                .append(" vertices, deleted ")
                .append(std::to_string(ctx->result_info_->statistics.edges_deleted))
                .append(" edges.");
            // CYPHER_THROW_ASSERT(ctx->result_->Header().size() == 1);
            CYPHER_THROW_ASSERT(record);
            record->values.clear();
            record->AddConstant(lgraph::FieldData(summary));
        } else {
            /* There should be a "Project" operation on top of this "Delete",
             * so leave result set to it. */
        }
    }

 public:
    std::string view_path_;
    std::set<std::string> view_names_;
    std::set<std::string> vertex_maintenances_;
    std::set<std::string> edge_maintenances_;
    bool is_view_=false;
    int edge_deleted_=0;
    
    OpDelete(const parser::QueryPart *stmt, PatternGraph *pattern_graph)
        : OpBase(OpType::DELETE_, "Delete"), pattern_graph_(pattern_graph) {
        delete_data_ = *stmt->delete_clause;
        state = StreamUnInitialized;
    }

    OpDelete(const parser::QueryPart *stmt, PatternGraph *pattern_graph,std::string view_path,bool is_view)
        : OpBase(OpType::DELETE_, "Delete"), pattern_graph_(pattern_graph),view_path_(view_path), is_view_(is_view){
        _SetViewInfs(view_path_);
        delete_data_ = *stmt->delete_clause;
        state = StreamUnInitialized;
    }

    OpResult Initialize(RTContext *ctx) override {
        CYPHER_THROW_ASSERT(parent && children.size() < 2);
        summary_ = !parent->parent;
        for (auto child : children) {
            child->Initialize(ctx);
        }
        record = children[0]->record;
        return OP_OK;
    }

    OpResult RealConsume(RTContext *ctx) override {
        if (state == StreamDepleted) return OP_DEPLETED;
        if (children.size() != 1) CYPHER_INTL_ERR();
        auto child = children[0];
        /* Avoid reading after writing (RAW) by split reading-while-writing
         * into 2 steps:
         * 1. collect vertices&edges to delete, this op store the
         *    vids/edge_uids while child op move iterator to next;
         * 2. do the delete action;  */
        while (child->Consume(ctx) == OP_OK) CollectVEToDelete();
        DoDeleteVE(ctx);
        ResultSummary(ctx);
        state = StreamDepleted;
        return OP_OK;
    }

    OpResult ResetImpl(bool complete) override {
        state = StreamUnInitialized;
        return OP_OK;
    }

    std::string ToString() const override {
        std::string str(name);
        return str;
    }

    const parser::Clause::TYPE_DELETE& GetDeleteData() const { return delete_data_; }

    PatternGraph* GetPatternGraph() const { return pattern_graph_; }

    CYPHER_DEFINE_VISITABLE()

    CYPHER_DEFINE_CONST_VISITABLE()
};
}  // namespace cypher
