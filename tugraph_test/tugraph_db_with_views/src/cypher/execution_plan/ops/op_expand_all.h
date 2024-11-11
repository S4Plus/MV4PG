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
// Created by wt on 7/3/18.
//
#pragma once

#include <unordered_set>
#include "cypher/execution_plan/ops/op.h"
#include "filter/filter.h"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second); 

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

namespace cypher {

/* ExpandAll
 * Expands entire graph,
 * Each node within the graph will be set */
class ExpandAll : public OpBase {
    friend class EdgeFilterPushdownExpand;

    const std::set<std::string> _GetViewTypes(std::string view_path){
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
        std::set<std::string> view_types;
        if(j.size()>0){
            for(auto element:j.at(0).items()){
                view_types.emplace(element.key());
            }
        }
        return view_types;
    }
    void _InitializeEdgeIter(RTContext *ctx) {
        auto &types = relp_->Types();
        auto iter_type = lgraph::EIter::NA;
        // switch (expand_direction_) {
        // case ExpandTowards::FORWARD:
        //     iter_type = types.empty() ? lgraph::EIter::OUT_EDGE : lgraph::EIter::TYPE_OUT_EDGE;
        //     break;
        // case ExpandTowards::REVERSED:
        //     iter_type = types.empty() ? lgraph::EIter::IN_EDGE : lgraph::EIter::TYPE_IN_EDGE;
        //     break;
        // case ExpandTowards::BIDIRECTIONAL:
        //     iter_type = types.empty() ? lgraph::EIter::BI_EDGE : lgraph::EIter::BI_TYPE_EDGE;
        //     break;
        // }
        //////////////////////
        // if(view_types_.empty())view_types_=_GetViewTypes(ctx);
        switch (expand_direction_) {
        case ExpandTowards::FORWARD:
            iter_type = types.empty() ? lgraph::EIter::VIEW_OUT_EDGE : lgraph::EIter::TYPE_OUT_EDGE;
            break;
        case ExpandTowards::REVERSED:
            iter_type = types.empty() ? lgraph::EIter::VIEW_IN_EDGE : lgraph::EIter::TYPE_IN_EDGE;
            break;
        case ExpandTowards::BIDIRECTIONAL:
            iter_type = types.empty() ? lgraph::EIter::BI_VIEW_EDGE : lgraph::EIter::BI_TYPE_EDGE;
            break;
        }
        // LOG_DEBUG()<<"view list:"<<*(_GetViewTypes(ctx).begin());
        // LOG_DEBUG()<<"initialize edge";
        if(types.empty()){
            // LOG_DEBUG()<<"view_types empty:"<<view_types_.empty();
            eit_->Initialize(ctx->txn_->GetTxn().get(), iter_type, start_->PullVid(), view_types_);
        }
        else
        //////////////////////
            eit_->Initialize(ctx->txn_->GetTxn().get(), iter_type, start_->PullVid(), types);
        // LOG_DEBUG()<<"initialize edge end";
        if(profile_)stats.db_hit++;
    }

    bool _CheckToSkipEdgeFilter(RTContext *ctx) const {
        // if the query has edge_filter, filter before node_expand
        auto start = std::chrono::high_resolution_clock::now();
        bool is_filter = edge_filter_ && !edge_filter_->DoFilter(ctx, *children[0]->record);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        check_filter_+=elapsed.count();
        return is_filter;
    }

    bool _CheckToSkipEdge(RTContext *ctx) const {
        return eit_->IsValid() &&
               (pattern_graph_->VisitedEdges().Contains(*eit_) || _CheckToSkipEdgeFilter(ctx) ||
                (expand_into_ && eit_->GetNbr(expand_direction_) != neighbor_->PullVid())
                || _CheckIfDuplicate(ctx) );
    }

    bool _CheckIfDuplicate(RTContext *ctx) const {
        auto start = std::chrono::high_resolution_clock::now();
        if(!no_dup_edge){return false;}
        if(ctx->deleted_view_edges.find(eit_->GetUid().ToString())!=ctx->deleted_view_edges.end()){
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            check_duplicate_+=elapsed.count();
            return true;
        }
        else{
            if(limit_delete_num>=0 && now_delete_num>=limit_delete_num)return true;
            else return false;
            // return expand_pair_node.find(std::make_pair(start_->PullVid(),eit_->GetNbr(expand_direction_)))
            //                 !=expand_pair_node.end();
        }
    }

    bool _FilterNeighborLabel(RTContext *ctx) {
        if (neighbor_->Label().empty()) return true;
        auto nbr_it = ctx->txn_->GetTxn()->GetVertexIterator(eit_->GetNbr(expand_direction_));
        while (ctx->txn_->GetTxn()->GetVertexLabel(nbr_it) != neighbor_->Label()) {
            eit_->Next();
            if(profile_)stats.db_hit++;
            if (!eit_->IsValid()) return false;
            nbr_it.Goto(eit_->GetNbr(expand_direction_));
            if(profile_)stats.db_hit++;
            CYPHER_THROW_ASSERT(nbr_it.IsValid());
        }
        return true;
    }

    void _DumpForDebug() const {
#ifndef NDEBUG
        LOG_DEBUG() << "[" << __FILE__ << "] start:" << start_->PullVid()
                  << ", neighbor:" << neighbor_->PullVid();
        LOG_DEBUG() << pattern_graph_->VisitedEdges().Dump();
#endif
    }

    OpResult Next(RTContext *ctx) {
        // Reset iterators
        // LOG_DEBUG() << "ExpandAll::Next";
        if (state_ == ExpandAllResetted) {
            /* Start node iterator may be invalid, such as when the start is an argument
             * produced by OPTIONAL MATCH.  */
            if (start_->PullVid() < 0) return OP_REFRESH;
            _InitializeEdgeIter(ctx);
            while (_CheckToSkipEdge(ctx)) {
                eit_->Next();
                if(profile_)stats.db_hit++;
            }
            if (!eit_->IsValid() || !_FilterNeighborLabel(ctx)) return OP_REFRESH;
            /* When relationship is undirected, GetNbr() will get src for out_edge_iterator
             * and dst for in_edge_iterator.  */
            neighbor_->PushVid(eit_->GetNbr(expand_direction_));
            if(profile_)stats.db_hit++;
            // if(view_types_.find(eit_->GetLabel())==view_types_.end())
            if(ctx->path_unique_)pattern_graph_->VisitedEdges().Add(*eit_);
            state_ = ExpandAllConsuming;
            // _DumpForDebug();
            return OP_OK;
        }
        // The iterators are set, keep on consuming.
        if(ctx->path_unique_)pattern_graph_->VisitedEdges().Erase(*eit_);
        do {
            eit_->Next();
            if(profile_)stats.db_hit++;
        } while (_CheckToSkipEdge(ctx));

        if (!eit_->IsValid() || !_FilterNeighborLabel(ctx)) return OP_REFRESH;
        neighbor_->PushVid(eit_->GetNbr(expand_direction_));
        if(profile_)stats.db_hit++;
        if(ctx->path_unique_)pattern_graph_->VisitedEdges().Add(*eit_);
        // _DumpForDebug();
        return OP_OK;
    }

 public:
    cypher::Node *start_;         // start node to expand
    cypher::Node *neighbor_;      // neighbor of start node
    cypher::Relationship *relp_;  // relationship to expand
    lgraph::EIter *eit_;
    int start_rec_idx_;
    int nbr_rec_idx_;
    int relp_rec_idx_;
    cypher::PatternGraph *pattern_graph_;
    bool expand_into_;
    ExpandTowards expand_direction_;
    std::shared_ptr<lgraph::Filter> edge_filter_ = nullptr;

    std::string view_path_;
    std::set<std::string> view_types_;
    bool no_dup_edge = false;
    // bool no_dup_find = false;
    int limit_delete_num=-1;
    int now_delete_num=0;
    mutable double check_duplicate_=0;
    mutable double check_filter_=0;
    std::unordered_set<std::pair<lgraph::VertexId,lgraph::VertexId>, pair_hash> expand_pair_node;
    // std::unordered_set<std::string> deleted_view_edges;
    /* ExpandAllStates
     * Different states in which ExpandAll can be at. */
    enum ExpandAllState {
        ExpandAllUninitialized, /* ExpandAll wasn't initialized it. */
        ExpandAllResetted,      /* ExpandAll was just restarted. */
        ExpandAllConsuming,     /* ExpandAll consuming data. */
    } state_;

    // TODO(anyone) rename expandAll to expand
    ExpandAll(PatternGraph *pattern_graph, Node *start, Node *neighbor, Relationship *relp,
              std::shared_ptr<lgraph::Filter> edge_filter = nullptr)
        : OpBase(OpType::EXPAND_ALL, "Expand"),
          start_(start),
          neighbor_(neighbor),
          relp_(relp),
          pattern_graph_(pattern_graph),
          edge_filter_(edge_filter) {
        throw lgraph::CypherException("We need view path now");
        CYPHER_THROW_ASSERT(start && neighbor && relp);
        eit_ = relp->ItRef();
        modifies.emplace_back(neighbor_->Alias());
        modifies.emplace_back(relp_->Alias());
        auto &sym_tab = pattern_graph->symbol_table;
        auto sit = sym_tab.symbols.find(start_->Alias());
        auto nit = sym_tab.symbols.find(neighbor_->Alias());
        auto rit = sym_tab.symbols.find(relp_->Alias());
        CYPHER_THROW_ASSERT(sit != sym_tab.symbols.end() && nit != sym_tab.symbols.end() &&
                            rit != sym_tab.symbols.end());
        expand_into_ = nit->second.scope == SymbolNode::ARGUMENT;
        expand_direction_ = relp_->Undirected()            ? BIDIRECTIONAL
                            : relp_->Src() == start_->ID() ? FORWARD
                                                           : REVERSED;
        start_rec_idx_ = sit->second.id;
        nbr_rec_idx_ = nit->second.id;
        relp_rec_idx_ = rit->second.id;
        state_ = ExpandAllUninitialized;
        no_dup_edge = relp_->no_duplicate_edge_;
    }

    ExpandAll(PatternGraph *pattern_graph, Node *start, Node *neighbor, Relationship *relp,
              std::string view_path, std::shared_ptr<lgraph::Filter> edge_filter = nullptr)
        : OpBase(OpType::EXPAND_ALL, "Expand"),
          start_(start),
          neighbor_(neighbor),
          relp_(relp),
          pattern_graph_(pattern_graph),
          edge_filter_(edge_filter),
          view_path_(view_path) {
        CYPHER_THROW_ASSERT(start && neighbor && relp);
        view_types_ = _GetViewTypes(view_path_);
        eit_ = relp->ItRef();
        modifies.emplace_back(neighbor_->Alias());
        modifies.emplace_back(relp_->Alias());
        auto &sym_tab = pattern_graph->symbol_table;
        auto sit = sym_tab.symbols.find(start_->Alias());
        auto nit = sym_tab.symbols.find(neighbor_->Alias());
        auto rit = sym_tab.symbols.find(relp_->Alias());
        CYPHER_THROW_ASSERT(sit != sym_tab.symbols.end() && nit != sym_tab.symbols.end() &&
                            rit != sym_tab.symbols.end());
        expand_into_ = nit->second.scope == SymbolNode::ARGUMENT;
        expand_direction_ = relp_->Undirected()            ? BIDIRECTIONAL
                            : relp_->Src() == start_->ID() ? FORWARD
                                                           : REVERSED;
        start_rec_idx_ = sit->second.id;
        nbr_rec_idx_ = nit->second.id;
        relp_rec_idx_ = rit->second.id;
        state_ = ExpandAllUninitialized;
        no_dup_edge = relp_->no_duplicate_edge_;
    }

    void PushDownEdgeFilter(std::shared_ptr<lgraph::Filter> edge_filter) {
        edge_filter_ = edge_filter;
    }

    OpResult Initialize(RTContext *ctx) override {
#ifndef NDEBUG
        LOG_DEBUG()<<"expand init";
#endif
        CYPHER_THROW_ASSERT(!children.empty());
#ifndef NDEBUG
        LOG_DEBUG()<<"expand grand parent size:"<<parent->parent->children.size();
#endif
        auto child = children[0];
#ifndef NDEBUG
        LOG_DEBUG()<<"expand grand parent size3:"<<parent->parent->children.size();
#endif
        auto res = child->Initialize(ctx);
#ifndef NDEBUG
        LOG_DEBUG()<<"expand grand parent size4:"<<parent->parent->children.size();
#endif
        if (res != OP_OK) return res;
        record = child->record;
        record->values[start_rec_idx_].type = Entry::NODE;
        record->values[start_rec_idx_].node = start_;
        record->values[nbr_rec_idx_].type = Entry::NODE;
        record->values[nbr_rec_idx_].node = neighbor_;
        record->values[relp_rec_idx_].type = Entry::RELATIONSHIP;
        record->values[relp_rec_idx_].relationship = relp_;
#ifndef NDEBUG
        LOG_DEBUG()<<"expand grand parent size2:"<<parent->parent->children.size();
        LOG_DEBUG()<<"expand init end";
#endif
        return OP_OK;
    }

    OpResult RealConsume(RTContext *ctx) override {
#ifndef NDEBUG
        LOG_DEBUG()<<"expand consume";
#endif
        CYPHER_THROW_ASSERT(!children.empty());
        auto child = children[0];
        // if(no_dup_edge && no_dup_find){
        //     no_dup_find=false;
        //     return OP_REFRESH;
        // }
        while (state_ == ExpandAllUninitialized || Next(ctx) == OP_REFRESH) {
            now_delete_num=0;
            // expand_pair_node.clear();
            auto res = child->Consume(ctx);
            state_ = ExpandAllResetted;
            if (res != OP_OK) {
                /* When consume after the stream is DEPLETED, make sure
                 * the result always be DEPLETED.  */
                state_ = ExpandAllUninitialized;
                return res;
            }
            /* Most of the time, the start_it is definitely valid after child's Consume
             * returns OK, except when the child is an OPTIONAL operation.  */
        }
        if(no_dup_edge && start_->PullVid()>=0 && neighbor_->PullVid()>=0){
            // no_dup_find=true;
            ctx->deleted_view_edges.emplace(eit_->GetUid().ToString());
            now_delete_num++;
            // expand_pair_node.emplace(start_->PullVid(),neighbor_->PullVid());
#ifndef NDEBUG
            LOG_DEBUG()<<"expand pair:"<<start_->PullVid()<<","<<neighbor_->PullVid();
#endif
            // return OP_REFRESH;
        }
        return OP_OK;
    }

    OpResult ResetImpl(bool complete) override {
        /* TODO(anyone) optimize, the apply operator need reset rhs stream completely,
         * while the cartesian product doesn't.
         * e.g.:
         * match (n:Person {name:'Vanessa Redgrave'})-->(m) with m as m1
         * match (n:Person {name:'Vanessa Redgrave'})<--(m) return m as m2, m1
         * */
        /* reset modifies */
        eit_->FreeIter();
        neighbor_->PushVid(-1);
        pattern_graph_->VisitedEdges().Erase(*eit_);
        state_ = ExpandAllUninitialized;
        now_delete_num=0;
        // expand_pair_node.clear();
        return OP_OK;
    }

    std::string ToString() const override {
        auto towards = expand_direction_ == FORWARD    ? "-->"
                       : expand_direction_ == REVERSED ? "<--"
                                                       : "--";
        std::string edgefilter_str = "EdgeFilter";
        return fma_common::StringFormatter::Format(
            "{}({}) [{} {} {} {},{},{}]", name, expand_into_ ? "Into" : "All", start_->Alias(), towards,
            neighbor_->Alias(),
            edge_filter_ ? edgefilter_str.append(" (").append(edge_filter_->ToString()).append(")")
                         : "", std::to_string(check_duplicate_),std::to_string(check_filter_));
    }

    Node* GetStartNode() const { return start_; }
    Node* GetNeighborNode() const { return neighbor_; }
    Relationship* GetRelationship() const { return relp_; }

    CYPHER_DEFINE_VISITABLE()

    CYPHER_DEFINE_CONST_VISITABLE()
};
}  // namespace cypher
