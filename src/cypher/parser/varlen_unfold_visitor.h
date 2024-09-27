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
#include "cypher/parser/data_typedef.h"

#if __APPLE__
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#endif  // #if __APPLE__
#define ANONYMOUS "ANON"
#define VAR_LEN_EXPAND_MAX_HOP 128
namespace parser {

/**
 * This class provides an empty implementation of LcypherVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class VarlenUnfoldVisitor : public LcypherVisitor {
    // size_t curr_pattern_graph = 0;  // 在整个Cypher中的第几个pattern_graph
    // // 点
    // std::string _label,_primary_key,_primary_value;
    // bool _value_is_string;
    // // 边
    // std::string _edge_label,_edge_variable;
    // size_t _edge_id;
    // NODE_INF _src_inf,_dst_inf; 
    // bool is_src;

    // bool _change=false; //控制给哪个点或边加index
    // bool _is_create,_is_vertex;
    std::vector<std::string> rewrite_queries;
    // std::string view_variable_;
    // std::vector<std::string> _relp_types;
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
        std::string alias(ANONYMOUS);
        if (is_node) {
            alias.append("N").append(std::to_string(_anonymous_idx));
        } else {
            alias.append("R").append(std::to_string(_anonymous_idx));
        }
        _anonymous_idx++;
        return alias;
    }

 public:
    
    VarlenUnfoldVisitor() = default;
    VarlenUnfoldVisitor(antlr4::tree::ParseTree *tree){
        tree->accept(this);
    }


    std::string visitChildrenToString(antlr4::tree::ParseTree *node){
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
    std::vector<std::string> GetRewriteQueries() const { return rewrite_queries; }
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

    std::any visitOC_Construct(LcypherParser::OC_ConstructContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_View(LcypherParser::OC_ViewContext *ctx) override {
        // std::cout <<"View start"<<std::endl;
        // view_name=ctx->oC_LabelName()->getText();
        // // view_name=std::any_cast<std::string>(visit(ctx->StringLiteral()));
        // std::cout <<view_name;
        // return std::any_cast<std::string>(visit(ctx->oC_RegularQuery()));
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
        return visitChildren(ctx);
    }

    std::any visitOC_SingleQuery(LcypherParser::OC_SingleQueryContext *ctx) override {
        return visitChildren(ctx);
    }

    std::any visitOC_SinglePartQuery(LcypherParser::OC_SinglePartQueryContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_MultiPartQuery(LcypherParser::OC_MultiPartQueryContext *ctx) override {
        if(ctx->oC_ReadingClause().size()==0)
            // throw ("no match clause");
            throw lgraph::CypherException("no match clause");
        if(ctx->oC_ReadingClause(0)->oC_Match()==nullptr)
            // throw ("no match clause");
            throw lgraph::CypherException("no match clause");
        std::cout<<"varlen unfold match s"<<std::endl;
        std::vector<std::string> match_queries=std::any_cast<std::vector<std::string>>(visit(ctx->oC_ReadingClause(0)->oC_Match()));
        std::cout<<"match s"<<std::endl;
        std::string oc_with=std::any_cast<std::string>(visit(ctx->oC_With(0)));
        std::cout<<"match s"<<std::endl;
        std::string update=std::any_cast<std::string>(visit(ctx->oC_SinglePartQuery()));
        std::cout<<"match s"<<std::endl;
        for(auto& mq:match_queries){
            mq.append(" "+oc_with);
            mq.append(" "+update);
            rewrite_queries.push_back(mq);
        }
        return 0;
    }

    std::any visitOC_UpdatingClause(LcypherParser::OC_UpdatingClauseContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_ReadingClause(LcypherParser::OC_ReadingClauseContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_Match(LcypherParser::OC_MatchContext *ctx) override {
        _EnterClauseMATCH();
        std::vector<std::string> pattern_elements=std::any_cast<std::vector<std::string>>(visit(ctx->oC_Pattern()->oC_PatternPart(0)->oC_AnonymousPatternPart()->oC_PatternElement()));
        for(auto& pe:pattern_elements){
            pe="match "+pe;
        }
        if(ctx->oC_Where()!=nullptr)
            throw lgraph::CypherException("where clause is not supported in views yet");
        
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

    std::any visitOC_Create(LcypherParser::OC_CreateContext *ctx) override {
        _EnterClauseCREATE();
        return visitChildrenToString(ctx);
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
        return visitChildrenToString(ctx);
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

    std::any visitOC_Pattern(LcypherParser::OC_PatternContext *ctx) override {
        // return visit(ctx->oC_PatternPart(0)->oC_AnonymousPatternPart()->oC_PatternElement());
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PatternPart(LcypherParser::OC_PatternPartContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_AnonymousPatternPart(
        LcypherParser::OC_AnonymousPatternPartContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_PatternElement(LcypherParser::OC_PatternElementContext *ctx) override {
        if(_InClauseMATCH()){
            std::vector<std::string> pattern_elements;
            if(ctx->oC_PatternElement()==nullptr){          
                std::string node_pattern=std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
                int pch_size=ctx->oC_PatternElementChain().size();
                if(pch_size==0){
                    pattern_elements.push_back(node_pattern);
                    return pattern_elements;
                }
                std::vector<std::vector<std::string>> all_pattern_chains;
                std::vector<int> indexs,sizes; //indexs保存每个pch现在遍历到哪，每个pch的size
                for(auto pch:ctx->oC_PatternElementChain()){
                    auto pattern_chains=std::any_cast<std::vector<std::string>>(visit(pch));
                    all_pattern_chains.push_back(pattern_chains);
                    indexs.push_back(0);
                    sizes.push_back(pattern_chains.size());
                }
                while(true){
                    std::string result;
                    result.append(node_pattern);
                    for(int i=0;i<pch_size;i++){
                        result.append(all_pattern_chains[i][indexs[i]]);
                    }
                    pattern_elements.push_back(result);
                    int i=pch_size-1;
                    while(i>=0){
                        if(indexs[i]<sizes[i]-1){
                            indexs[i]++;
                            break;
                        }
                        else{
                            indexs[i]=0;
                            i--;
                        }
                    }
                    if(i<0)break;
                }
            }
            else
                return visit(ctx->oC_PatternElement());
                // for(auto pe:pattern_elements){
                //     std::cout<<"match: "<<pe<<std::endl;
                // }
                // std::cout<<"pe size: "<<pattern_elements.size()<<std::endl;
            return pattern_elements;
        }
        return visitChildrenToString(ctx);
    }

    std::any visitOC_NodePattern(LcypherParser::OC_NodePatternContext *ctx) override {
        return visitChildrenToString(ctx);
        // std::string variable;
        // std::string result = "(";

        // std::string node_label,primary_key,primary_value;
        // bool is_string=false;
        // if(_change){
        //     if(_is_vertex){
        //         node_label=_label;
        //         primary_key=_primary_key;
        //         primary_value=_primary_value;
        //         is_string=_value_is_string;
        //     }
        //     else{
        //         if(is_src){
        //             node_label=std::get<0>(_src_inf);
        //             primary_key=std::get<1>(_src_inf);
        //             primary_value=std::get<2>(_src_inf);
        //             is_string=std::get<3>(_src_inf);
        //         }
        //         else{
        //             node_label=std::get<0>(_dst_inf);
        //             primary_key=std::get<1>(_dst_inf);
        //             primary_value=std::get<2>(_dst_inf);
        //             is_string=std::get<3>(_dst_inf);
        //         }
        //     }
        // }
        // if (ctx->oC_Variable() != nullptr) {
        //     variable = std::any_cast<std::string>(visit(ctx->oC_Variable()));
        //     result.append(variable);
        // } else {
        //     // if alias is absent, generate an alias for the node
        //     variable = GenAnonymousAlias(true);
        // }
        // std::string label;
        // if (ctx->oC_NodeLabels() != nullptr) {
        //     label=std::any_cast<std::string>(visit(ctx->oC_NodeLabels()));
        //     label.erase(0,1);
        // }
        // if(_change){
        //     if(label!=node_label && !label.empty())return std::string();
        //     else result.append(":"+node_label);
        // }
        // else if(!label.empty())result.append(":"+label);
        // // auto node = &(pattern_graphs_[curr_pattern_graph].GetNode(variable));
        // // if (!node->Label().empty()) {
        // //     result.append(":" + node->Label());
        // // }
        // if (ctx->oC_Properties() != nullptr) {
        //     if(_change)
        //         // throw ("views do not support node properties now");
        //         throw lgraph::CypherException("views do not support node properties now");
        //     else{           
        //         std::string properties = std::any_cast<std::string>(visit(ctx->oC_Properties()));
        //         result.append(properties);
        //     }
        // }
        // else{
        //     if(_change){
        //         if(is_string)
        //             result.append("{"+primary_key+":'"+primary_value+"'}");
        //         else
        //             result.append("{"+primary_key+":"+primary_value+"}");
        //     }
        // }
        // result.append(")");
        // return result;
    }

    std::any visitOC_PatternElementChain(LcypherParser::OC_PatternElementChainContext *ctx) override {
        if(_InClauseMATCH()){
            std::string end_node_pattern=std::any_cast<std::string>(visit(ctx->oC_NodePattern()));
            auto relp_pattern=ctx->oC_RelationshipPattern();
            std::string left_arrow,right_arrow;
            if(relp_pattern->oC_LeftArrowHead()!=nullptr){
                left_arrow=relp_pattern->oC_LeftArrowHead()->getText()+relp_pattern->oC_Dash(0)->getText();
            }
            else left_arrow=relp_pattern->oC_Dash(0)->getText();
            if(relp_pattern->oC_RightArrowHead()!=nullptr){
                right_arrow=relp_pattern->oC_Dash(1)->getText()+relp_pattern->oC_RightArrowHead()->getText();
            }
            else right_arrow=relp_pattern->oC_Dash(1)->getText();
            auto relp_detail=std::any_cast<std::string>(visit(relp_pattern->oC_RelationshipDetail()));
            std::vector<std::string> unfold_relps;
            if(relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral()==nullptr){
                std::string unfold_relp;
                unfold_relp.append(left_arrow);
                unfold_relp.append(relp_detail);
                unfold_relp.append(right_arrow);
                unfold_relp.append(end_node_pattern);
                unfold_relps.push_back(unfold_relp);
            }
            else{
                auto range_literal=std::any_cast<std::array<int, 2>>(visit(relp_pattern->oC_RelationshipDetail()->oC_RangeLiteral()));
                int min_hop=std::max(range_literal[0],0);
                int max_hop=std::max(range_literal[1],0);
                for(int hop=min_hop;hop<=max_hop;hop++){
                    std::string unfold_relp;
                    if(hop==0)unfold_relps.push_back("");
                    else{
                        for(int i=1;i<=hop;i++){
                            unfold_relp.append(left_arrow);
                            unfold_relp.append(relp_detail);
                            unfold_relp.append(right_arrow);
                            if(i<hop)
                                unfold_relp.append("()");
                            else
                                unfold_relp.append(end_node_pattern);
                        }
                        unfold_relps.push_back(unfold_relp);
                    }
                }
            }
            return unfold_relps;
        }
        else return visitChildrenToString(ctx);
    }

    std::any visitOC_RelationshipPattern(LcypherParser::OC_RelationshipPatternContext *ctx) override {
        return visitChildrenToString(ctx);
        // std::string left_arrow,right_arrow;
        // if(ctx->oC_LeftArrowHead()!=nullptr){
        //     left_arrow=ctx->oC_LeftArrowHead()->getText()+ctx->oC_Dash(0)->getText();
        // }
        // else left_arrow=ctx->oC_Dash(0)->getText();
        // if(ctx->oC_RightArrowHead()!=nullptr){
        //     right_arrow=ctx->oC_Dash(1)->getText()+ctx->oC_RightArrowHead()->getText();
        // }
        // else right_arrow=ctx->oC_Dash(1)->getText();
        // auto relp_detail=std::any_cast<std::string>(visit(ctx->oC_RelationshipDetail()));
        // std::vector<std::string> unfold_relp;
        // if(ctx->oC_RelationshipDetail()->oC_RangeLiteral()==nullptr){
        //     unfold_relp.push_back(relp_detail);
        //     return;
        // }
        // else{
        //     auto range_literal=std::any_cast<std::array<int, 2>>(visit(ctx->oC_RelationshipDetail()->oC_RangeLiteral()));
        //     int min_hop=max(range_literal[0],0);
        // }
        // std::string result;
        // LinkDirection direction;
        // std::string relationship_detail;
        // if (ctx->oC_RelationshipDetail() != nullptr) {
        //     auto relp_detail_with_direction = std::any_cast<std::tuple<LinkDirection, std::string>>(
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

    std::any visitOC_RelationshipDetail(LcypherParser::OC_RelationshipDetailContext *ctx) override {
        std::string result;
        std::string variable;
        result.append("[");
        if (ctx->oC_Variable() != nullptr) {
            variable = std::any_cast<std::string>(visit(ctx->oC_Variable()));
            // result.append(variable);
        } else {
            // if alias is absent, generate an alias for the relationship
            variable = GenAnonymousAlias(false);
            // result.append(variable);
        }
        if(!_InClauseMATCH())result.append(variable);
        if(ctx->oC_RelationshipTypes()!=nullptr){
            std::string types=std::any_cast<std::string>(visit(ctx->oC_RelationshipTypes()));
            result.append(types);
        }
        
        // if(ctx->oC_RangeLiteral()!=nullptr)
        //     throw lgraph::CypherException("range literal is not supported in views yet");
        
        if (ctx->oC_Properties() != nullptr) {
            std::string properties = std::any_cast<std::string>(visit(ctx->oC_Properties()));
            result.append(properties);
        }
        result.append("]");
        return result;
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

    std::any visitOC_Properties(LcypherParser::OC_PropertiesContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_RelationshipTypes(LcypherParser::OC_RelationshipTypesContext *ctx) override {
        return visitChildrenToString(ctx);
    }

    std::any visitOC_NodeLabels(LcypherParser::OC_NodeLabelsContext *ctx) override {
        if(ctx->oC_NodeLabel().size()>1){
            // throw("more than one node label is not supported yet");
            throw lgraph::CypherException("more than one node label is not supported yet");
        }
        return visit(ctx->oC_NodeLabel(0));
    }

    std::any visitOC_NodeLabel(LcypherParser::OC_NodeLabelContext *ctx) override {
        std::string result=":";
        result.append(std::any_cast<std::string>(visit(ctx->oC_LabelName())));
        return visitChildrenToString(ctx);
    }

    std::any visitOC_RangeLiteral(LcypherParser::OC_RangeLiteralContext *ctx) override {
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
                    CYPHER_THROW_ASSERT(valid_children.at(1)->getText() == "..");
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
                    CYPHER_THROW_ASSERT(valid_children.at(1)->getText() == "..");
                    range_literal[0] = 1;
                    range_literal[1] = std::stoi(ctx->oC_IntegerLiteral(0)->getText());
                }
                break;
            }
        case 4:
            {
                CYPHER_THROW_ASSERT(ctx->oC_IntegerLiteral().size() == 2);
                range_literal[0] = std::stoi(ctx->oC_IntegerLiteral(0)->getText());
                range_literal[1] = std::stoi(ctx->oC_IntegerLiteral(1)->getText());
                break;
            }
        default:
            // throw("not implemented yet");
            CYPHER_TODO();
        }
        return range_literal;
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

    std::any visitOC_Dash(LcypherParser::OC_DashContext *ctx) override {
        return ctx->getText();
    }

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
