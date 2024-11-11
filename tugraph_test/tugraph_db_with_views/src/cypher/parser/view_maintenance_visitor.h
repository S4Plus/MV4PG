/**
 * Copyright 2022 AntGroup CO., Ltd.
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

// Generated from Lcypher.g4 by ANTLR 4.9.2

#pragma once

#include <cassert>
#include <tuple>
#include "antlr4-runtime/antlr4-runtime.h"

#include "generated/LcypherVisitor.h"
#include "cypher/cypher_exception.h"

#if __APPLE__
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#endif  // #if __APPLE__

#define NODE_INF                                      \
    std::tuple<std::string, std::string, std::string, \
               bool>  // label,primary_key,primary_value,is_string
#define VAR_LEN_EXPAND_MAX_HOP 128
namespace parser {

/**
 * This class provides an empty implementation of LcypherVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class ViewMaintenance : public LcypherVisitor {
    size_t curr_pattern_graph = 0;  // 在整个Cypher中的第几个pattern_graph
    // 点
    std::string _node_ids;
    std::string _node_variable="VN";
    std::string node_name;
    // 边
    std::string _edge_variable, _edge_ids;
    std::string _src_variable="VS",_dst_variable="VD"; // 可变长中的
    std::string _src_ids, _dst_ids;
    std::string src_name,dst_name; // 非可变长中的
    bool is_src;
    bool is_unfold = false;
    bool is_inneed = false;
    bool _change = false;  // 控制给哪个点或边加index
    bool _is_create, _is_vertex;
    std::vector<std::string> rewrite_queries;
    std::string view_variable_;
    std::vector<std::string> _relp_types;
    /* Anonymous entity are not in symbol table:
     * MATCH (n) RETURN exists((n)-->()-->())  */
    size_t _anonymous_idx = 0;
    enum _ClauseType : uint32_t {
        NA = 0x0,
        MATCH = 0x1,
        RETURN = 0x2,
        WITH = 0x4,
        UNWIND = 0x8,
        WHERE = 0x10,
        ORDERBY = 0x20,
        CREATE = 0x40,
        DELETE = 0x80,
        SET = 0x100,
        REMOVE = 0x200,
        MERGE = 0x400,
        INQUERYCALL = 0x800,
    } _curr_clause_type = NA;

    std::string GenAnonymousAlias(bool is_node) {
        std::string alias("ANON_");
        if (is_node) {
            alias.append("N").append(std::to_string(_anonymous_idx));
        } else {
            alias.append("R").append(std::to_string(_anonymous_idx));
        }
        _anonymous_idx++;
        return alias;
    }

 public:
    ViewMaintenance() = default;
    // 点
    ViewMaintenance(antlr4::tree::ParseTree *tree, std::string node_ids, bool is_create)
        : _node_ids(node_ids) {
        // _value_is_string = value_is_string;
        _is_create = is_create;
        _is_vertex = true;
        tree->accept(this);
    }
    // 边
    ViewMaintenance(antlr4::tree::ParseTree *tree, std::string edge_variable, std::string edge_ids,
                    std::string src_ids, std::string dst_ids, bool is_create)
        : _edge_variable(edge_variable), _edge_ids(edge_ids), _src_ids(src_ids), _dst_ids(dst_ids) {
        _is_create = is_create;
        _is_vertex = false;
        tree->accept(this);
    }

    std::string visitChildrenToString(antlr4::tree::ParseTree *node) {
        std::string result;
        for (auto child : node->children) {
            std::string child_text;
            if (child->children.size() > 0)
                child_text = std::any_cast<std::string>(visit(child));
            else
                child_text = child->getText();
            result.append(child_text);
        }
        return result;
    }
    std::string GetRewriteQuery() const {
        std::string result = "";
        for (size_t i = 0; i < rewrite_queries.size(); i++) {
            if (i > 0) result += "\nUnion \n";
            result += rewrite_queries[i];
        }
        return "Maintenance " + result;
    }
    std::vector<std::string> GetRewriteQueries() const {
        return rewrite_queries;
        // std::string result="";
        // for(size_t i=0;i<rewrite_queries.size();i++){
        //     if(i>0)result+="\nUnion All\n";
        //     result+=rewrite_queries[i];
        // }
        // return result;
    }
    // std::string GetViewName() const { return view_name; }

    std::any visitOC_Cypher(LcypherParser::OC_CypherContext *ctx) override {
        // std::cout <<"Cypher start"<<std::endl;
        // rewrite_query = std::any_cast<std::string>(visit(ctx->oC_Statement()));
        visit(ctx->oC_Statement());
        return 0;
    }

    std::any visitOC_Statement(LcypherParser::OC_StatementContext *ctx) override {
        // std::cout <<"Statement start"<<std::endl;
        // if(ctx->oC_View()==nullptr){
        //     throw lgraph::CypherException("Not Views");
        // }
        return visitChildren(ctx);
    }

    std::any visitOC_Query(LcypherParser::OC_QueryContext *ctx) override {
        return visitChildren(ctx);
    }

    // std::any visitOC_Views(LcypherParser::OC_ViewsContext *ctx) override {
    //     return visitChildren(ctx);
    // }

    std::any visitOC_View(LcypherParser::OC_ViewContext *ctx) override {
        // std::cout <<"View start"<<std::endl;
        // view_name=ctx->oC_LabelName()->getText();
        // // view_name=std::any_cast<std::string>(visit(ctx->StringLiteral()));
        // std::cout <<view_name;
        // return std::any_cast<std::string>(visit(ctx->oC_RegularQuery()));
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Construct(LcypherParser::OC_ConstructContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_RegularQuery(LcypherParser::OC_RegularQueryContext *ctx) override {
        // reserve for single_queries
        // std::cout <<"RQ s"<<std::endl;
        _anonymous_idx = 0;
        return visitChildren(ctx);
    }

    std::any visitOC_Union(LcypherParser::OC_UnionContext *ctx) override {
        _anonymous_idx = 0;
        curr_pattern_graph++;
        return visitChildren(ctx);
    }
    // oC_SingleQuery : oC_SinglePartQuery
    //                | oC_MultiPartQuery
    //                ;
    std::any visitOC_SingleQuery(LcypherParser::OC_SingleQueryContext *ctx) override {
        if (ctx->oC_MultiPartQuery() == nullptr) throw("need Multi Part Query");
        // throw lgraph::CypherException("need Multi Part Query");
        return visitChildren(ctx);
    }
    // oC_SinglePartQuery : ( ( oC_ReadingClause SP? )* oC_Return )
    //                    | ( ( oC_ReadingClause SP? )* oC_UpdatingClause ( SP? oC_UpdatingClause )*
    //                    ( SP? oC_Return )? )
    //                    ;
    std::any visitOC_SinglePartQuery(LcypherParser::OC_SinglePartQueryContext *ctx) override {
        return visitChildrenToString(ctx);
    }
    // oC_MultiPartQuery : ( ( oC_ReadingClause SP? )* ( oC_UpdatingClause SP? )* oC_With SP? )+
    // oC_SinglePartQuery ;
    std::any visitOC_MultiPartQuery(LcypherParser::OC_MultiPartQueryContext *ctx) override {
        if (ctx->oC_ReadingClause().size() == 0) throw("no match clause");
        // throw lgraph::CypherException("no match clause");
        if (ctx->oC_ReadingClause(0)->oC_Match() == nullptr) throw("no match clause");
        // throw lgraph::CypherException("no match clause");
        // std::cout<<"match s"<<std::endl;
        std::vector<std::string> match_queries =
            std::any_cast<std::vector<std::string>>(visit(ctx->oC_ReadingClause(0)->oC_Match()));
        // std::cout<<"match s"<<std::endl;
        std::string oc_with = std::any_cast<std::string>(visit(ctx->oC_With(0)));
        // std::cout<<"match s"<<std::endl;
        std::string update = std::any_cast<std::string>(visit(ctx->oC_SinglePartQuery()));
        // std::cout<<"match s"<<std::endl;
        for (auto &mq : match_queries) {
            mq.append(" " + oc_with);
            if(!_is_create)mq.append(",count(*) as num");
            mq.append(" " + update);
            rewrite_queries.push_back(mq);
        }
        return 0;
    }  // 接受with语句和最后的creat语句
    // oC_UpdatingClause : oC_Create
    //                   | oC_Merge
    //                   | oC_Delete
    //                   | oC_Set
    //                   | oC_Remove
    //                   ;
    std::any visitOC_UpdatingClause(LcypherParser::OC_UpdatingClauseContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ReadingClause(LcypherParser::OC_ReadingClauseContext *ctx) override {
        return visitChildrenToString(ctx);
    }
    // oC_Match : ( OPTIONAL_ SP )? MATCH SP? oC_Pattern ( SP? oC_Hint )* ( SP? oC_Where )? ;
    std::any visitOC_Match(LcypherParser::OC_MatchContext *ctx) override {
        _EnterClauseMATCH();
        std::vector<std::string> pattern_elements = std::any_cast<std::vector<std::string>>(visit(
            ctx->oC_Pattern()->oC_PatternPart(0)->oC_AnonymousPatternPart()->oC_PatternElement()));
        for (auto &pe : pattern_elements) {
            pe = "match " + pe;
        }
        if (ctx->oC_Where() != nullptr)
            // throw lgraph::CypherException("where clause is not supported in views yet");
            throw("where clause is not supported in views yet");

        _LeaveClauseMATCH();
        return pattern_elements;
    }

    std::any visitOC_Unwind(LcypherParser::OC_UnwindContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Merge(LcypherParser::OC_MergeContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_MergeAction(LcypherParser::OC_MergeActionContext *ctx) override {
        return visitChildrenToString(ctx);
    }
    // oC_Create : CREATE SP? oC_Pattern ;
    std::any visitOC_Create(LcypherParser::OC_CreateContext *ctx) override {
        _EnterClauseCREATE();
        // std::cout<<"create"<<std::endl;
        if (_is_create)
            return visitChildrenToString(ctx);
        else {
            std::string pattern = std::any_cast<std::string>(visit(ctx->oC_Pattern()));
            std::string result = "match " + pattern + " delete " + view_variable_;
            return result;
        }
        _LeaveClauseCREATE();
    }

    std::any visitOC_Set(LcypherParser::OC_SetContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_SetItem(LcypherParser::OC_SetItemContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Delete(LcypherParser::OC_DeleteContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Remove(LcypherParser::OC_RemoveContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_RemoveItem(LcypherParser::OC_RemoveItemContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_InQueryCall(LcypherParser::OC_InQueryCallContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_StandaloneCall(LcypherParser::OC_StandaloneCallContext *ctx) override {
        _anonymous_idx = 0;
        return visitChildrenToString(ctx);
    }

    std::any visitOC_YieldItems(LcypherParser::OC_YieldItemsContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_YieldItem(LcypherParser::OC_YieldItemContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_With(LcypherParser::OC_WithContext *ctx) override {
        std::string result = visitChildrenToString(ctx);
        curr_pattern_graph++;  //????
        return result;
    }

    std::any visitOC_Return(LcypherParser::OC_ReturnContext *ctx) override {
        return visitChildrenToString(ctx);
        // std::cout <<"Return s"<<std::endl;
        // std::string result;
        // result="WITH ";
        // if(ctx->DISTINCT()!=nullptr){
        //     result.append(ctx->DISTINCT()->getText());
        //     result.append(" ");
        // }
        // result.append(std::any_cast<std::string>(visit(ctx->oC_ReturnBody())));
        // std::cout <<"Return e"<<std::endl;
        // return result;
    }

    std::any visitOC_ReturnBody(LcypherParser::OC_ReturnBodyContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ReturnItems(LcypherParser::OC_ReturnItemsContext *ctx) override {
        return visitChildrenToString(ctx);
        // std::cout <<"Return item s"<<std::endl;
        // // if(ctx->oC_ReturnItem().size()!=2){
        // //     throw lgraph::CypherException("Views now only support two return nodes");
        // // }
        // // std::string start=std::any_cast<std::string>(visit(ctx->oC_ReturnItem(0)));
        // // std::string end=std::any_cast<std::string>(visit(ctx->oC_ReturnItem(1)));
        // // const cypher::Node empty;
        // // auto start_node = &(pattern_graphs_[curr_pattern_graph].GetNode(start));
        // // auto end_node = &(pattern_graphs_[curr_pattern_graph].GetNode(end));
        // // // if(start_node->Label().empty()||end_node->Label().empty())
        // // //     throw lgraph::CypherException("Views need explicit node labels");
        // // constraints.first=start_node->Label();
        // // constraints.second=end_node->Label();
        // // std::string result=start+","+end+" ";
        // // result.append("CREATE ("+start+")-[r:"+view_name+"]->("+end+")");
        // std::string result="CREATE (n:"+label+" { "+primary_key+":'"+primary_value+"' })";
        // return result;
    }

    std::any visitOC_ReturnItem(LcypherParser::OC_ReturnItemContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Order(LcypherParser::OC_OrderContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Skip(LcypherParser::OC_SkipContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Limit(LcypherParser::OC_LimitContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_SortItem(LcypherParser::OC_SortItemContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Hint(LcypherParser::OC_HintContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Where(LcypherParser::OC_WhereContext *ctx) override {
        return visitChildrenToString(ctx);
    }
    // oC_Pattern : oC_PatternPart ( SP? ',' SP? oC_PatternPart )* ;
    std::any visitOC_Pattern(LcypherParser::OC_PatternContext *ctx) override {
        // return visit(ctx->oC_PatternPart(0)->oC_AnonymousPatternPart()->oC_PatternElement());
        return visitChildrenToString(ctx);
    }
    // oC_PatternPart : ( oC_Variable SP? '=' SP? oC_AnonymousPatternPart )
    //                | oC_AnonymousPatternPart
    //                ;
    std::any visitOC_PatternPart(LcypherParser::OC_PatternPartContext *ctx) override {
        return visitChildrenToString(ctx);
    }
    // oC_AnonymousPatternPart : oC_PatternElement ;
    std::any visitOC_AnonymousPatternPart(
        LcypherParser::OC_AnonymousPatternPartContext *ctx) override {
        return visitChildrenToString(ctx);
    }
    // oC_PatternElement : ( oC_NodePattern ( SP? oC_PatternElementChain )* )
    //                   | ( '(' oC_PatternElement ')' )
    //                   ;
    std::string vetostring(std::vector<std::string> t) {
        std::string result;
        for (auto it : t) {
            result.append(it);
        }
        return result;
    }

    LinkDirection GetDirection(LcypherParser::OC_RelationshipPatternContext *ctx) {
        LinkDirection direction;
        if (ctx->oC_LeftArrowHead() != nullptr && ctx->oC_RightArrowHead() == nullptr) {
            direction = LinkDirection::RIGHT_TO_LEFT;
        } else if (ctx->oC_RightArrowHead() != nullptr && ctx->oC_LeftArrowHead() == nullptr) {
            direction = LinkDirection::LEFT_TO_RIGHT;
        } else {
            direction = LinkDirection::DIR_NOT_SPECIFIED;
        }
        return direction;
    }

    std::any visitOC_PatternElement(LcypherParser::OC_PatternElementContext *ctx) override {
        if (_InClauseMATCH()) {
            std::vector<std::string> pattern_elements;
            if (ctx->oC_NodePattern() != nullptr) {
                if (_is_vertex) {  // 操作点
                    is_unfold = false;
                    int node_size = ctx->oC_PatternElementChain().size() + 1;
                    for (int i = 0; i < (node_size); i++) {
                        std::string result;
                        if (i == 0) {
                            bool can_rewrite = true;
                            // std::string result;
                            _change = true;
                            std::string node_pattern =
                                std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                            // std::cout<<"node pattern: "<<node_pattern<<std::endl;
                            if (node_pattern.empty()) can_rewrite = false;
                            result.append(node_pattern);
                            _change = false;
                            for (auto pch : ctx->oC_PatternElementChain()) {
                                std::string pattern_chain = std::any_cast<std::string>(visit(pch));
                                result.append(pattern_chain);
                            }
                            result.append(" where id("+node_name+") in "+_node_ids+" ");
                            if (can_rewrite) pattern_elements.push_back(result);
                        } else {
                            bool can_rewrite = true;
                            // std::string result;
                            std::string node_pattern =
                                std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                            result.append(node_pattern);
                            // std::cout<<"node pattern 1: "<<node_pattern<<std::endl;
                            int now_node = 1;
                            for (auto pch : ctx->oC_PatternElementChain()) {
                                std::string relp_pattern = std::any_cast<std::string>(
                                    visit(pch->oC_RelationshipPattern()));
                                std::string node_pattern;
                                if (now_node == i) _change = true;
                                node_pattern =
                                    std::any_cast<std::string>(visit(pch->oC_NodePattern()));
                                // if(_change)std::cout<<"node pattern: "<<node_pattern<<std::endl;
                                if (node_pattern.empty()) can_rewrite = false;
                                _change = false;
                                result.append(relp_pattern);
                                result.append(node_pattern);
                                now_node++;
                            }
                            result.append(" where id("+node_name+") in "+_node_ids+" ");
                            if (can_rewrite) pattern_elements.push_back(result);
                        }
                    }
                    // 可变长展开
                    std::vector<std::vector<std::string>> all_pattern_chains;
                    std::vector<int> path_size;

                    for (auto pch : ctx->oC_PatternElementChain()) {
                        is_unfold = true;
                        auto pattern_chains = std::any_cast<std::vector<std::string>>(visit(pch));
                        is_unfold = false;
                        // for(auto it:pattern_chains)std::cout<<it<<std::endl;
                        all_pattern_chains.push_back(pattern_chains);
                        path_size.push_back(pattern_chains.size());
                    }

                    std::vector<std::string> results;
                    std::string node_pattern =
                        std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                    results.push_back(node_pattern);
                    for (size_t i = 0; i < all_pattern_chains.size(); i++) {
                        results.push_back(all_pattern_chains[i][0]);
                    }
                    for (size_t i = 0; i < all_pattern_chains.size(); i++) {
                        int size = all_pattern_chains[i].size();
                        if (size != 1) {
                            for (int j = 1; j < size; j++) {
                                results[i + 1] = all_pattern_chains[i][j];
                                std::string result = vetostring(results);
                                result.append(" where id(" + _node_variable + ") in " + _node_ids);
                                pattern_elements.push_back(result);
                            }
                            results[i + 1] = all_pattern_chains[i][0];
                        }
                    }

                } else {
                    int edge_size = ctx->oC_PatternElementChain().size();
                    for (size_t i = 0; i < (size_t)edge_size; i++) {
                        auto relp_Pattern =
                            ctx->oC_PatternElementChain(i)->oC_RelationshipPattern();
                        if (i == 0) {
                            std::string result;
                            bool can_rewrite = true;
                            _change = true;
                            is_src = (GetDirection(relp_Pattern) == LinkDirection::LEFT_TO_RIGHT);
                            std::string src_node_pattern =
                                std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                            if (src_node_pattern.empty()) can_rewrite = false;
                            result.append(src_node_pattern);

                            _change = false;
                            for (size_t j = 0; j < ctx->oC_PatternElementChain().size(); j++) {
                                if (j == i) {
                                    _change = true;
                                    std::string edge_pattern = std::any_cast<std::string>(
                                        visit(relp_Pattern->oC_RelationshipDetail()));
                                    if (edge_pattern.empty()) can_rewrite = false;
                                    edge_pattern = std::any_cast<std::string>(visit(relp_Pattern));
                                    result.append(edge_pattern);
                                    is_src = !(GetDirection(relp_Pattern) ==
                                               LinkDirection::LEFT_TO_RIGHT);
                                    ;
                                    std::string dst_node_pattern = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)->oC_NodePattern()));
                                    if (dst_node_pattern.empty()) can_rewrite = false;
                                    result.append(dst_node_pattern);
                                    _change = false;
                                } else {
                                    std::string pattern_chain = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)));
                                    result.append(pattern_chain);
                                }
                            }
                            result.append(" where euid(" + _edge_variable + ") in " + _edge_ids);
                            result.append(" and id("+src_name+") in "+_src_ids+" ");
                            // result.append(" and id("+dst_name+") in "+_dst_ids+" ");
                            if (can_rewrite) pattern_elements.push_back(result);
                        } else {
                            std::string result;
                            bool can_rewrite = true;
                            std::string start =
                                std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                            result.append(start);
                            for (size_t j = 0; j < ctx->oC_PatternElementChain().size(); j++) {
                                if (j == (i - 1)) {
                                    std::string edge_pattern = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)
                                                  ->oC_RelationshipPattern()
                                                  ->oC_RelationshipDetail()));
                                    edge_pattern = std::any_cast<std::string>(visit(
                                        ctx->oC_PatternElementChain(j)->oC_RelationshipPattern()));
                                    result.append(edge_pattern);
                                    _change = true;
                                    is_src = (GetDirection(relp_Pattern) ==
                                              LinkDirection::LEFT_TO_RIGHT);
                                    ;
                                    std::string src_node_pattern = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)->oC_NodePattern()));
                                    if (src_node_pattern.empty()) can_rewrite = false;
                                    result.append(src_node_pattern);
                                    _change = false;
                                } else if (j == i) {
                                    _change = true;
                                    std::string edge_pattern = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)
                                                  ->oC_RelationshipPattern()
                                                  ->oC_RelationshipDetail()));
                                    if (edge_pattern.empty()) can_rewrite = false;
                                    edge_pattern = std::any_cast<std::string>(visit(
                                        ctx->oC_PatternElementChain(j)->oC_RelationshipPattern()));
                                    result.append(edge_pattern);
                                    is_src = !(GetDirection(relp_Pattern) ==
                                               LinkDirection::LEFT_TO_RIGHT);
                                    ;
                                    std::string dst_node_pattern = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)->oC_NodePattern()));
                                    if (dst_node_pattern.empty()) can_rewrite = false;
                                    result.append(dst_node_pattern);
                                    _change = false;
                                } else {
                                    std::string pattern_chain = std::any_cast<std::string>(
                                        visit(ctx->oC_PatternElementChain(j)));
                                    result.append(pattern_chain);
                                }
                            }
                            result.append(" where euid(" + _edge_variable + ") in " + _edge_ids);
                            result.append(" and id("+src_name+") in "+_src_ids+" ");
                            // result.append(" and id("+dst_name+") in "+_dst_ids+" ");
                            if (can_rewrite) pattern_elements.push_back(result);
                        }
                    }
                    // 可变长
                    std::vector<std::vector<std::string>> all_pattern_chains;
                    std::vector<int> path_size;
                    for (auto pch : ctx->oC_PatternElementChain()) {
                        is_unfold = true;
                        auto pattern_chains = std::any_cast<std::vector<std::string>>(visit(pch));
                        // for(auto it:pattern_chains)std::cout<<it<<std::endl;
                        is_unfold = false;
                        all_pattern_chains.push_back(pattern_chains);
                        path_size.push_back(pattern_chains.size());
                    }
                    std::vector<std::string> results;
                    std::string node_pattern =
                        std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                    results.push_back(node_pattern);
                    for (auto it : all_pattern_chains) {
                        results.push_back(it[0]);
                    }
                    for (size_t i = 0; i < all_pattern_chains.size(); i++) {
                        int size = all_pattern_chains[i].size();
                        if (size != 1) {
                            for (int j = 1; j < size; j++) {
                                results[i + 1] = all_pattern_chains[i][j];
                                std::string result = vetostring(results);
                                result.append(" where euid(" + _edge_variable + ") in " + _edge_ids);
                                result.append(" and id("+_src_variable+") in "+_src_ids+" ");
                                // result.append(" and id("+_dst_variable+") in "+_dst_ids+" ");
                                pattern_elements.push_back(result);
                            }
                            results[i + 1] = all_pattern_chains[i][0];
                        }
                    }
                }
            } else
                return visit(ctx->oC_PatternElement());
            // for(auto pe:pattern_elements){
            //     std::cout<<"match: "<<pe<<std::endl;
            // }
            // std::cout<<"pe size: "<<pattern_elements.size()<<std::endl;
            return pattern_elements;
        } else
            return visitChildrenToString(ctx);
    }
    // oC_NodePattern : '(' SP? ( oC_Variable SP? )? ( oC_NodeLabels SP? )? ( oC_Properties SP? )?
    // ')' ;
    std::any visitOC_NodePattern(LcypherParser::OC_NodePatternContext *ctx) override {
        // return visitChildrenToString(ctx);
        std::string variable;
        std::string result = "(";

        std::string node_label, primary_key, primary_value;
        // bool is_string = false;
        // if (_change) {
        //     if (_is_vertex) {
        //         node_label = _label;
        //         primary_key = _primary_key;
        //         primary_value = _primary_value;
        //         is_string = _value_is_string;
        //     } 
        //     // else {
        //     //     if (is_src) {
        //     //         node_label = std::get<0>(_src_inf);
        //     //         primary_key = std::get<1>(_src_inf);
        //     //         primary_value = std::get<2>(_src_inf);
        //     //         is_string = std::get<3>(_src_inf);
        //     //     } else {
        //     //         node_label = std::get<0>(_dst_inf);
        //     //         primary_key = std::get<1>(_dst_inf);
        //     //         primary_value = std::get<2>(_dst_inf);
        //     //         is_string = std::get<3>(_dst_inf);
        //     //     }
        //     // }
        // }
        if (ctx->oC_Variable() != nullptr) {
            variable = std::any_cast<std::string>(visit(ctx->oC_Variable()));
            result.append(variable);
        } else {
            // if alias is absent, generate an alias for the node
            variable = GenAnonymousAlias(true);
            result.append(variable);
        }
        if (_change ) {
            if(_is_vertex)node_name=variable;
            else{
                if(is_src)src_name=variable;
                else dst_name=variable;
            }
        }
        std::string label;
        if (ctx->oC_NodeLabels() != nullptr) {
            label = std::any_cast<std::string>(visit(ctx->oC_NodeLabels()));
            result.append(label);
        }
        // if (_change) {
        //     result.append(":" + node_label);
        // };
        // auto node = &(pattern_graphs_[curr_pattern_graph].GetNode(variable));
        // if (!node->Label().empty()) {
        //     result.append(":" + node->Label());
        // }
        if (ctx->oC_Properties() != nullptr) {
            if (_change) throw("views do not support node properties now");
            // throw lgraph::CypherException("views do not support node properties now");
            else {
                std::string properties = std::any_cast<std::string>(visit(ctx->oC_Properties()));
                result.append(properties);
            }
        } 
        // else {
        //     if (_change) {
        //         if (is_string)
        //             result.append("{" + primary_key + ":'" + primary_value + "'}");
        //         else
        //             result.append("{" + primary_key + ":" + primary_value + "}");
        //     }
        // }
        result.append(")");
        return result;
    }
    std::string getvetex() {
        std::string result{"("};
        result.append(_node_variable+")");
        // // result.append(GenAnonymousAlias(true));
        // result.append(":").append(_label).append("{").append(_primary_key).append(":");
        // if (_value_is_string) result.append("'");
        // result.append(_primary_value);
        // if (_value_is_string) result.append("'");
        // result.append("})");
        return result;
    }
    // oC_PatternElementChain : oC_RelationshipPattern SP? oC_NodePattern ;
    std::any visitOC_PatternElementChain(
        LcypherParser::OC_PatternElementChainContext *ctx) override {
        if (is_unfold && _is_vertex) {
            std::string edgetypes = "";
            std::string end_node_pattern = std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
            auto relp_pattern = ctx->oC_RelationshipPattern();
            std::string left_arrow, right_arrow;
            if (relp_pattern->oC_LeftArrowHead() != nullptr) {
                left_arrow = relp_pattern->oC_LeftArrowHead()->getText() +
                             relp_pattern->oC_Dash(0)->getText();
            } else
                left_arrow = relp_pattern->oC_Dash(0)->getText();
            if (relp_pattern->oC_RightArrowHead() != nullptr) {
                right_arrow = relp_pattern->oC_Dash(1)->getText() +
                              relp_pattern->oC_RightArrowHead()->getText();
            } else
                right_arrow = relp_pattern->oC_Dash(1)->getText();
            auto relp_detail =
                std::any_cast<std::string>(visit(relp_pattern->oC_RelationshipDetail()));
            std::vector<std::string> unfold_relps;
            // if(relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral()==nullptr){
            std::string unfold_relp;
            unfold_relp.append(left_arrow);
            unfold_relp.append(relp_detail);
            unfold_relp.append(right_arrow);
            unfold_relp.append(end_node_pattern);
            unfold_relps.push_back(unfold_relp);
            //}
            if (relp_pattern->oC_RelationshipDetail()->oC_RelationshipTypes() != nullptr) {
                int index = 0;
                for (auto rt : relp_pattern->oC_RelationshipDetail()
                                   ->oC_RelationshipTypes()
                                   ->oC_RelTypeName()) {
                    if (index > 0) edgetypes.append("|");
                    edgetypes.append(std::any_cast<std::string>(visit(rt)));
                    index++;
                }
            }
            if (relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral() == nullptr)
                return unfold_relps;
            else {
                is_inneed = true;
                auto range_literal = std::any_cast<std::array<int, 2>>(
                    visit(relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral()));
                is_inneed = false;
                int min_hop = std::max(range_literal[0], 0);
                int max_hop = std::max(range_literal[1], 0);
                if (max_hop == VAR_LEN_EXPAND_MAX_HOP) {
                    // std::vector<std::string>all_unfold_path;

                    for (int i = 1; i < std::max(min_hop - 1, 1); i++) {
                        std::string path;
                        std::string inx1 = std::to_string(i);
                        std::string inx2 = std::to_string(min_hop - i);

                        path.append(left_arrow);
                        path.append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("").append("*");
                        path.append(inx1).append("]").append(right_arrow);
                        path.append(getvetex());  // 实现getvetex获得删除点(k)
                        path.append(left_arrow);
                        path.append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("*").append(inx2).append("..]");
                        path.append(right_arrow).append(end_node_pattern);
                        unfold_relps.push_back(path);
                    }
                    std::string inx = std::to_string(std::max(min_hop - 1, 1));
                    std::string path1;
                    path1.append(left_arrow);
                    path1.append("[");
                    if (!edgetypes.empty()) path1.append(":").append(edgetypes);
                    path1.append("").append("*");
                    path1.append(inx).append("..]").append(right_arrow);
                    path1.append(getvetex());  // 实现getvetex获得删除点(k)
                    path1.append(left_arrow);
                    path1.append("[");
                    if (!edgetypes.empty()) path1.append(":").append(edgetypes);
                    path1.append("*").append("1").append("..]");
                    path1.append(right_arrow).append(end_node_pattern);
                    unfold_relps.push_back(path1);
                } else {
                    for (int i = 1; i < max_hop; i++) {
                        std::string path;
                        std::string inx1 = std::to_string(i);
                        std::string inx2 = std::to_string(std::max(min_hop - i, 1));
                        std::string inx3 = std::to_string(max_hop - i);
                        path.append(left_arrow);
                        path.append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("").append("*");
                        path.append(inx1).append("]").append(right_arrow);
                        path.append(getvetex());  // 实现getvetex获得删除点(k)
                        path.append(left_arrow);
                        path.append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("*").append(inx2).append("..").append(inx3).append("]");
                        path.append(right_arrow).append(end_node_pattern);
                        unfold_relps.push_back(path);
                    }
                    //  for(int i=0;i<min_hop;i++){
                    //     if(i==0)continue;
                    //     else{
                    //         std::string path;
                    //         std::string inx1=std::to_string(i);
                    //         std::string inx2=std::to_string(min_hop-i);
                    //         std::string inx3=std::to_string(max_hop-i);
                    //         path.append(left_arrow);
                    //         path.append("[");
                    //     if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //         path.append("").append("*");
                    //         path.append(inx1).append("]->");
                    //         path.append(getvetex());//实现getvetex获得删除点(k)
                    //         path.append(right_arrow);
                    //         path.append("[");
                    //        if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //         path.append("*").append(inx2).append("..").append(inx3).append("]");
                    //         path.append(right_arrow).append(end_node_pattern);
                    //         unfold_relps.push_back(path);
                    //     }
                    //  }
                    // for(int i=min_hop;i<max_hop;i++)
                    // {
                    //     std::string path;
                    //     std::string inx1=std::to_string(i);
                    //     std::string inx3=std::to_string(max_hop-i);
                    //     path.append(left_arrow);
                    //     path.append("[");
                    //     if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //     path.append("").append("*");
                    //     path.append(inx1).append("]->");
                    //     path.append(getvetex());//实现getvetex获得删除点(k)
                    //     path.append(right_arrow);
                    //     path.append("[");
                    //    if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //     path.append("*").append("0").append("..").append(inx3).append("]");
                    //     path.append(right_arrow).append(end_node_pattern);
                    //     unfold_relps.push_back(path);
                    // }
                    //        std::string inx=std::to_string(max_hop);
                    //        std::string path1;
                    //        path1.append(left_arrow);
                    //        path1.append("[");
                    //         if(!edgetypes.empty())path1.append(":").append(edgetypes);

                    //        path1.append("").append("*");
                    //        path1.append(inx).append("]->");
                    //        path1.append(getvetex());//实现getvetex获得删除点(k)
                    //        path1.append(right_arrow);
                    //        path1.append("[");
                    //        if(!edgetypes.empty())path1.append(":").append(edgetypes);
                    //        path1.append("*").append("0").append("]");
                    //        path1.append(right_arrow).append(end_node_pattern);
                    //        unfold_relps.push_back(path1);
                }
                return unfold_relps;
            }
        }
        // 边
        if (is_unfold && !_is_vertex) {
            std::string edgetypes = "";
            std::string end_node_pattern = std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
            auto relp_pattern = ctx->oC_RelationshipPattern();
            std::string left_arrow, right_arrow;
            if (relp_pattern->oC_LeftArrowHead() != nullptr) {
                left_arrow = relp_pattern->oC_LeftArrowHead()->getText() +
                             relp_pattern->oC_Dash(0)->getText();
            } else
                left_arrow = relp_pattern->oC_Dash(0)->getText();
            if (relp_pattern->oC_RightArrowHead() != nullptr) {
                right_arrow = relp_pattern->oC_Dash(1)->getText() +
                              relp_pattern->oC_RightArrowHead()->getText();
            } else
                right_arrow = relp_pattern->oC_Dash(1)->getText();
            auto relp_detail =
                std::any_cast<std::string>(visit(relp_pattern->oC_RelationshipDetail()));
            std::vector<std::string> unfold_relps;
            // if(relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral()==nullptr){
            std::string unfold_relp;
            unfold_relp.append(left_arrow);
            unfold_relp.append(relp_detail);
            unfold_relp.append(right_arrow);
            unfold_relp.append(end_node_pattern);
            unfold_relps.push_back(unfold_relp);
            //}
            if (relp_pattern->oC_RelationshipDetail()->oC_RelationshipTypes() != nullptr) {
                // bool _is_=false;
                int index = 0;
                for (auto rt : relp_pattern->oC_RelationshipDetail()
                                   ->oC_RelationshipTypes()
                                   ->oC_RelTypeName()) {
                    if (index > 0) edgetypes.append("|");
                    edgetypes.append(std::any_cast<std::string>(visit(rt)));
                    index++;
                }
            }
            if (relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral() == nullptr)
                return unfold_relps;
            else {
                is_inneed = true;
                auto range_literal = std::any_cast<std::array<int, 2>>(
                    visit(relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral()));
                is_inneed = false;
                int min_hop = std::max(range_literal[0], 0);
                int max_hop = std::max(range_literal[1], 0);
                if (max_hop == VAR_LEN_EXPAND_MAX_HOP) {
                    for (int i = 0; i < std::max(min_hop - 1, 0); i++) {
                        std::string index1 = std::to_string(i);
                        std::string index2 = std::to_string(min_hop - 1 - i);
                        std::string path;
                        path.append(left_arrow).append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("*");
                        path.append(index1).append("]").append(right_arrow);
                        path.append(
                            getedge(edgetypes, GetDirection(relp_pattern)));  // 获得边(:person{id:1})-[r:write]->(:movie{id:2})
                        path.append(left_arrow).append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("*");
                        path.append(index2).append("..]");
                        path.append(right_arrow);
                        path.append(end_node_pattern);
                        unfold_relps.push_back(path);
                    }
                    std::string index1 = std::to_string(std::max(min_hop - 1, 0));
                    std::string path1;
                    path1.append(left_arrow).append("[");
                    if (!edgetypes.empty()) path1.append(":").append(edgetypes);
                    path1.append("*");
                    path1.append(index1).append("..]").append(right_arrow);
                    path1.append(
                        getedge(edgetypes, GetDirection(relp_pattern)));  // 获得边(:person{id:1})-[r:write]->(:movie{id:2})
                    path1.append(left_arrow).append("[");
                    if (!edgetypes.empty()) path1.append(":").append(edgetypes);
                    path1.append("*");
                    path1.append("0").append("..]");
                    path1.append(right_arrow);
                    path1.append(end_node_pattern);
                    if (min_hop - 1 >= 0) unfold_relps.push_back(path1);
                } else {
                    for (int i = 0; i < max_hop; i++) {
                        std::string path;
                        std::string index1 = std::to_string(i);
                        std::string index2 = std::to_string(std::max(min_hop - i - 1, 0));
                        std::string index3 = std::to_string(max_hop - i - 1);
                        path.append(left_arrow).append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("*");
                        path.append(index1);
                        path.append("]").append(right_arrow).append(getedge(edgetypes, GetDirection(relp_pattern)));
                        path.append(left_arrow).append("[");
                        if (!edgetypes.empty()) path.append(":").append(edgetypes);
                        path.append("*");
                        path.append(index2).append("..").append(index3).append("]");
                        path.append(right_arrow);
                        path.append(end_node_pattern);
                        unfold_relps.push_back(path);
                    }
                    // for(int i=0;i<min_hop;i++)
                    // {
                    //     std::string path;
                    //     std::string index1=std::to_string(i);
                    //     std::string index2=std::to_string(min_hop-i-1);
                    //     std::string index3=std::to_string(max_hop-i-1);
                    //     path.append(left_arrow).append("[");
                    //     if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //     path.append("*");
                    //     path.append(index1);
                    //     path.append("]->").append(getedge(edgetypes));
                    //     path.append(left_arrow).append("[");
                    //     if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //     path.append("*");
                    //     path.append(index2).append("..").append(index3).append("]");
                    //     path.append(right_arrow);
                    //     path.append(end_node_pattern);
                    //     unfold_relps.push_back(path);
                    // }
                    // for(int i=min_hop;i<max_hop-1;i++)
                    // {
                    //     std::string path;
                    //     std::string index1=std::to_string(i);
                    //     std::string index3=std::to_string(max_hop-i-1);
                    //     path.append(left_arrow).append("[");
                    //     if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //     path.append("*");
                    //     path.append(index1);
                    //     path.append("]->").append(getedge(edgetypes));
                    //     path.append(left_arrow).append("[");
                    //     if(!edgetypes.empty())path.append(":").append(edgetypes);
                    //     path.append("*");
                    //     path.append("0").append("..").append(index3).append("]");
                    //     path.append(right_arrow);
                    //     path.append(end_node_pattern);
                    //     unfold_relps.push_back(path);
                    // }
                    // std::string path1;
                    // std::string index1=std::to_string(max_hop-1);
                    // path1.append(left_arrow).append("[");
                    // if(!edgetypes.empty())path1.append(":").append(edgetypes);
                    // path1.append("*").append(index1);
                    // path1.append("]->").append(getedge(edgetypes));
                    // path1.append(left_arrow).append("[");
                    // if(!edgetypes.empty())path1.append(":").append(edgetypes);
                    // path1.append("*");
                    // path1.append("0").append("]");
                    // path1.append(right_arrow);
                    // path1.append(end_node_pattern);
                    // unfold_relps.push_back(path1);
                }
            }
            return unfold_relps;
        }
        return visitChildrenToString(ctx);
    }
    // oC_RelationshipPattern : ( oC_LeftArrowHead SP? oC_Dash SP? oC_RelationshipDetail? SP?
    // oC_Dash SP? oC_RightArrowHead )
    //                        | ( oC_LeftArrowHead SP? oC_Dash SP? oC_RelationshipDetail? SP?
    //                        oC_Dash ) | ( oC_Dash SP? oC_RelationshipDetail? SP? oC_Dash SP?
    //                        oC_RightArrowHead ) | ( oC_Dash SP? oC_RelationshipDetail? SP? oC_Dash
    //                        )
    //                        ;
    std::any visitOC_RelationshipPattern(
        LcypherParser::OC_RelationshipPatternContext *ctx) override {
        return visitChildrenToString(ctx);
        // std::string result;
        // LinkDirection direction;
        // std::string relationship_detail;
        // if (ctx->oC_RelationshipDetail() != nullptr) {
        //     auto relp_detail_with_direction = std::any_cast<std::tuple<LinkDirection,
        //     std::string>>(
        //         visit(ctx->oC_RelationshipDetail()));
        //     direction = std::get<0>(relp_detail_with_direction);
        //     relationship_detail = std::get<1>(relp_detail_with_direction);
        // } else {
        //     auto alias = GenAnonymousAlias(false);
        //     auto relp = &(pattern_graphs_[curr_pattern_graph].GetRelationship(alias));
        //     direction = relp->direction_;
        //     if (relp->Types().size() > 0) {
        //         relationship_detail.append("[:");
        //         int i = 0;
        //         for (auto type : relp->Types()) {
        //             if (i > 0) relationship_detail.append("|");
        //             relationship_detail.append(type);
        //             i++;
        //         }
        //         relationship_detail.append("]");
        //     }
        // }

        // switch (direction) {
        // case LinkDirection::RIGHT_TO_LEFT:
        //     result.append("<-");
        //     break;
        // case LinkDirection::LEFT_TO_RIGHT:
        //     result.append("-");
        //     break;
        // case LinkDirection::DIR_NOT_SPECIFIED:
        //     if (ctx->oC_LeftArrowHead() != nullptr) {
        //         result.append(ctx->oC_LeftArrowHead()->getText());
        //     }
        //     result.append(ctx->oC_Dash(0)->getText());
        //     break;
        // default:
        //     break;
        // }

        // result.append(relationship_detail);

        // switch (direction) {
        // case LinkDirection::RIGHT_TO_LEFT:
        //     result.append("-");
        //     break;
        // case LinkDirection::LEFT_TO_RIGHT:
        //     result.append("->");
        //     break;
        // case LinkDirection::DIR_NOT_SPECIFIED:
        //     result.append(ctx->oC_Dash(1)->getText());
        //     if (ctx->oC_RightArrowHead() != nullptr) {
        //         result.append(ctx->oC_RightArrowHead()->getText());
        //     }
        //     break;
        // default:
        //     break;
        // }

        // return result;
    }
    // oC_RelationshipDetail : '[' SP? ( oC_Variable SP? )? ( oC_RelationshipTypes SP? )?
    // oC_RangeLiteral? ( oC_Properties SP? )? ']' ;
    std::any visitOC_RelationshipDetail(LcypherParser::OC_RelationshipDetailContext *ctx) override {
        if (_InClauseCREATE()) {
            if (ctx->oC_Variable() == nullptr) throw("view relationship must have a variable");
            // throw lgraph::CypherException("relationship must have a variable");
            view_variable_ = std::any_cast<std::string>(visit(ctx->oC_Variable()));
            if (!_is_create) {
                std::string result;
                result.append("[");
                result.append(view_variable_);
                if (ctx->oC_RelationshipTypes() != nullptr) {
                    auto relp_types =
                        std::any_cast<std::string>(visit(ctx->oC_RelationshipTypes()));
                    result.append(relp_types);
                }
                if (ctx->oC_RangeLiteral() != nullptr) {
                    auto range_literal = std::any_cast<std::string>(visit(ctx->oC_RangeLiteral()));
                    result.append(range_literal);
                }
                if (ctx->oC_Properties() != nullptr) {
                    auto properties = std::any_cast<std::string>(visit(ctx->oC_Properties()));
                    result.append(properties);
                }
                result.append(" NoDupEdge ");
                result.append("]");
                return result;
            }
        } else if (_InClauseMATCH()) {
            if (_change) {
                std::string result;
                std::string variable;
                result.append("[");
                // if (ctx->oC_Variable() != nullptr) {
                //     variable = std::any_cast<std::string>(visit(ctx->oC_Variable()));
                //     result.append(variable);
                // } else {
                //     // if alias is absent, generate an alias for the relationship
                //     variable = GenAnonymousAlias(false);
                //     result.append(variable);
                // }
                // _edge_variable=variable;
                result.append(_edge_variable);
                _relp_types.clear();
                if (ctx->oC_RelationshipTypes() != nullptr) visit(ctx->oC_RelationshipTypes());
                if (!_relp_types.empty()) {
                    int index = 0;
                    for (auto it : _relp_types) {
                        if (index == 0)
                            result.append(":" + it);
                        else
                            result.append("|" + it);
                        index++;
                    }
                }
                if (ctx->oC_RangeLiteral() != nullptr)
                    // throw lgraph::CypherException("range literal is not supported in views yet");
                    return std::string();
                if (ctx->oC_Properties() != nullptr) {
                    std::string properties =
                        std::any_cast<std::string>(visit(ctx->oC_Properties()));
                    result.append(properties);
                }
                result.append("]");
                return result;
            }
        }
        return visitChildrenToString(ctx);
        // std::string result;
        // std::string variable;
        // result.append("[");
        // if (ctx->oC_Variable() != nullptr) {
        //     variable = std::any_cast<std::string>(visit(ctx->oC_Variable()));
        //     result.append(variable);
        // } else {
        //     // if alias is absent, generate an alias for the relationship
        //     variable = GenAnonymousAlias(false);
        // }
        // // There is no need to use oC_RelationshipTypes()
        // auto relp = &(pattern_graphs_[curr_pattern_graph].GetRelationship(variable));
        // // 可变长情况，匿名变量只有一条可变长path，直接展开
        // if (relp->GetPaths().size() == 1 && ctx->oC_Variable() == nullptr) {
        //     auto path = relp->GetPaths().at(0);
        //     for (size_t i = 1; i < path.m_vlabels.size(); i++) {
        //         auto direction = path.m_direction[i - 1];
        //         if (i > 1) {
        //             if (direction == parser::LinkDirection::RIGHT_TO_LEFT) {
        //                 result.append("<-[");
        //             } else {
        //                 result.append("-[");
        //             }
        //         }
        //         std::string label_str(":");
        //         std::set<std::string> elabels = path.m_elabels[i - 1];
        //         int j = 0;
        //         for (auto elabel : elabels) {
        //             if (j) label_str.append("|");
        //             label_str.append(elabel);
        //             j++;
        //         }

        //         result.append(label_str);
        //         result.append("]");
        //         if (i < path.m_vlabels.size() - 1) {
        //             if (direction == parser::LinkDirection::LEFT_TO_RIGHT) {
        //                 result.append("->(");
        //             } else {
        //                 result.append("-(");
        //             }
        //             result.append(path.m_vlabels[i]);
        //             result.append(")");
        //         }
        //     }
        // } else {
        //     if (relp->Types().size() > 0) {
        //         result.append(":");
        //     }
        //     int i = 0;
        //     for (auto type : relp->Types()) {
        //         if (i > 0) result.append("|");
        //         result.append(type);
        //         i++;
        //     }
        //     if (ctx->oC_RangeLiteral() != nullptr) {
        //         std::string range_literal =
        //             std::any_cast<std::string>(visit(ctx->oC_RangeLiteral()));
        //         result.append(range_literal);
        //     }
        //     if (ctx->oC_Properties() != nullptr) {
        //         std::string properties = std::any_cast<std::string>(visit(ctx->oC_Properties()));
        //         result.append(properties);
        //     }
        //     result.append("]");
        // }
        // return std::make_tuple(relp->direction_, result);
    }
    // oC_Properties : oC_MapLiteral
    //               | oC_Parameter
    //               ;
    std::any visitOC_Properties(LcypherParser::OC_PropertiesContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::string getedge(std::string edgetypes, LinkDirection direction)
    {
        std::string result;
        std::string node1{"("};
        std::string edge("[");
        std::string node2{"("};
        node1.append(_src_variable+")");
        // node1.append(":");
        // if (std::get<3>(_src_inf))
        //     node1.append("'");
        // node1.append(std::get<2>(_src_inf));
        // if (std::get<3>(_src_inf))
        //     node1.append("'");
        // node1.append("})");
        node2.append(_dst_variable+")");
        // node2.append(std::get<0>(_dst_inf)).append("{").append(std::get<1>(_dst_inf));
        // node2.append(":");
        // if (std::get<3>(_dst_inf))
        //     node2.append("'");
        // node2.append(std::get<2>(_dst_inf));
        // if (std::get<3>(_dst_inf))
        //     node2.append("'");
        // node2.append("})");
        edge.append(_edge_variable);
        if(edgetypes.size()>0)
            edge.append(":");
        edge.append(edgetypes);
        edge.append("]");
        if (direction == LinkDirection::LEFT_TO_RIGHT)
            result.append(node1).append("-").append(edge).append("->").append(node2);
        else
            result.append(node2).append("<-").append(edge).append("-").append(node1);
        return result;
    }

    std::any visitOC_RelationshipTypes(LcypherParser::OC_RelationshipTypesContext *ctx) override {
        for (auto rt : ctx->oC_RelTypeName()) {
            _relp_types.push_back(std::any_cast<std::string>(visit(rt)));
        }
        return visitChildrenToString(ctx);
    }
    // oC_NodeLabels : oC_NodeLabel ( SP? oC_NodeLabel )* ;
    std::any visitOC_NodeLabels(LcypherParser::OC_NodeLabelsContext *ctx) override {
        if (ctx->oC_NodeLabel().size() > 1) {
            throw("more than one node label is not supported yet");
            // throw lgraph::CypherException("more than one node label is not supported yet");
        }
        return visit(ctx->oC_NodeLabel(0));
    }
    // oC_NodeLabel : ':' SP? oC_LabelName ;
    std::any visitOC_NodeLabel(LcypherParser::OC_NodeLabelContext *ctx) override {
        std::string result = ":";
        result.append(std::any_cast<std::string>(visit(ctx->oC_LabelName())));
        return visitChildrenToString(ctx);
    }  //////////////?????
    // oC_RangeLiteral : '*' SP? ( oC_IntegerLiteral SP? )? ( '..' SP? ( oC_IntegerLiteral SP? )? )?
    // ;
    std::any visitOC_RangeLiteral(LcypherParser::OC_RangeLiteralContext *ctx) override {
        if (is_unfold && is_inneed) {
            std::array<int, 2> range_literal{-1, -1};
            std::vector<antlr4::tree::ParseTree *> valid_children;
            for (const auto &it : ctx->children) {
                const auto &text = it->getText();
                if (!std::all_of(text.cbegin(), text.cend(), ::isspace)) {
                    valid_children.emplace_back(it);
                }
            }
            switch (valid_children.size()) {
            case 2:
                {
                    if (!ctx->oC_IntegerLiteral().empty()) {
                        range_literal[0] = std::stoi(ctx->oC_IntegerLiteral(0)->getText());
                        range_literal[1] = range_literal[0];
                    } else {
                        // CYPHER_THROW_ASSERT(valid_children.at(1)->getText() == "..");
                        range_literal[0] = 1;
                        range_literal[1] = VAR_LEN_EXPAND_MAX_HOP;
                    }
                    break;
                }
            case 3:
                {
                    if (valid_children.at(2)->getText() == "..") {
                        range_literal[0] = std::stoi(ctx->oC_IntegerLiteral(0)->getText());
                        range_literal[1] = VAR_LEN_EXPAND_MAX_HOP;
                    } else {
                        // CYPHER_THROW_ASSERT(valid_children.at(1)->getText() == "..");
                        range_literal[0] = 1;
                        range_literal[1] = std::stoi(ctx->oC_IntegerLiteral(0)->getText());
                    }
                    break;
                }
            case 4:
                {
                    // CYPHER_THROW_ASSERT(ctx->oC_IntegerLiteral().size() == 2);
                    range_literal[0] = std::stoi(ctx->oC_IntegerLiteral(0)->getText());
                    range_literal[1] = std::stoi(ctx->oC_IntegerLiteral(1)->getText());
                    break;
                }
            default:
                throw("not implemented yet");
                // CYPHER_TODO();
            }
            return range_literal;
        }
        return visitChildrenToString(ctx);
    }

    std::any visitOC_LabelName(LcypherParser::OC_LabelNameContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_RelTypeName(LcypherParser::OC_RelTypeNameContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Expression(LcypherParser::OC_ExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_OrExpression(LcypherParser::OC_OrExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_XorExpression(LcypherParser::OC_XorExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_AndExpression(LcypherParser::OC_AndExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_NotExpression(LcypherParser::OC_NotExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ComparisonExpression(
        LcypherParser::OC_ComparisonExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_AddOrSubtractExpression(
        LcypherParser::OC_AddOrSubtractExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_MultiplyDivideModuloExpression(
        LcypherParser::OC_MultiplyDivideModuloExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PowerOfExpression(LcypherParser::OC_PowerOfExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_UnaryAddOrSubtractExpression(
        LcypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_StringListNullOperatorExpression(
        LcypherParser::OC_StringListNullOperatorExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ListOperatorExpression(
        LcypherParser::OC_ListOperatorExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_StringOperatorExpression(
        LcypherParser::OC_StringOperatorExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_NullOperatorExpression(
        LcypherParser::OC_NullOperatorExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PropertyOrLabelsExpression(
        LcypherParser::OC_PropertyOrLabelsExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Atom(LcypherParser::OC_AtomContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Literal(LcypherParser::OC_LiteralContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_BooleanLiteral(LcypherParser::OC_BooleanLiteralContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ListLiteral(LcypherParser::OC_ListLiteralContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PartialComparisonExpression(
        LcypherParser::OC_PartialComparisonExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ParenthesizedExpression(
        LcypherParser::OC_ParenthesizedExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_RelationshipsPattern(
        LcypherParser::OC_RelationshipsPatternContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_FilterExpression(LcypherParser::OC_FilterExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_IdInColl(LcypherParser::OC_IdInCollContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_FunctionInvocation(LcypherParser::OC_FunctionInvocationContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_FunctionName(LcypherParser::OC_FunctionNameContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ExplicitProcedureInvocation(
        LcypherParser::OC_ExplicitProcedureInvocationContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ImplicitProcedureInvocation(
        LcypherParser::OC_ImplicitProcedureInvocationContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ProcedureResultField(
        LcypherParser::OC_ProcedureResultFieldContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ProcedureName(LcypherParser::OC_ProcedureNameContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Namespace(LcypherParser::OC_NamespaceContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ListComprehension(LcypherParser::OC_ListComprehensionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PatternComprehension(
        LcypherParser::OC_PatternComprehensionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PropertyLookup(LcypherParser::OC_PropertyLookupContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_CaseExpression(LcypherParser::OC_CaseExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_CaseAlternatives(LcypherParser::OC_CaseAlternativesContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Variable(LcypherParser::OC_VariableContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_NumberLiteral(LcypherParser::OC_NumberLiteralContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_MapLiteral(LcypherParser::OC_MapLiteralContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Parameter(LcypherParser::OC_ParameterContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_PropertyExpression(LcypherParser::OC_PropertyExpressionContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PropertyKeyName(LcypherParser::OC_PropertyKeyNameContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_IntegerLiteral(LcypherParser::OC_IntegerLiteralContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_DoubleLiteral(LcypherParser::OC_DoubleLiteralContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_SchemaName(LcypherParser::OC_SchemaNameContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ReservedWord(LcypherParser::OC_ReservedWordContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_SymbolicName(LcypherParser::OC_SymbolicNameContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_LeftArrowHead(LcypherParser::OC_LeftArrowHeadContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_RightArrowHead(LcypherParser::OC_RightArrowHeadContext *ctx) override {
        return ctx->getText();
    }

    std::any visitOC_Dash(LcypherParser::OC_DashContext *ctx) override { return ctx->getText(); }

#define CLAUSE_HELPER_FUNC(clause)                                                   \
    inline bool _InClause##clause() {                                                \
        return (_curr_clause_type & static_cast<_ClauseType>(clause)) != NA;         \
    }                                                                                \
    inline void _EnterClause##clause() {                                             \
        _curr_clause_type = static_cast<_ClauseType>(_curr_clause_type | clause);    \
    }                                                                                \
    inline void _LeaveClause##clause() {                                             \
        _curr_clause_type = static_cast<_ClauseType>(_curr_clause_type & (~clause)); \
    }

    CLAUSE_HELPER_FUNC(MATCH);
    CLAUSE_HELPER_FUNC(RETURN);
    CLAUSE_HELPER_FUNC(WITH);
    CLAUSE_HELPER_FUNC(UNWIND);
    CLAUSE_HELPER_FUNC(WHERE);
    CLAUSE_HELPER_FUNC(ORDERBY);
    CLAUSE_HELPER_FUNC(CREATE);
    CLAUSE_HELPER_FUNC(DELETE);
    CLAUSE_HELPER_FUNC(SET);
    CLAUSE_HELPER_FUNC(REMOVE);
    CLAUSE_HELPER_FUNC(MERGE);
    CLAUSE_HELPER_FUNC(INQUERYCALL);
};

}  // namespace parser
