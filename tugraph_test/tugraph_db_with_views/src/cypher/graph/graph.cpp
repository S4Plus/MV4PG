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
// Created by wt on 6/12/18.
//

#include <iostream>
#include <stack>
#include "fma-common/utils.h"
#include "parser/clause.h"
#include "parser/symbol_table.h"
#include "cypher/graph/graph.h"

namespace cypher {

PatternGraph::PatternGraph() : _next_nid(0), _next_rid(0) {}

void PatternGraph::_CollectExpandStepsByDFS(NodeID start, bool ignore_created,
                                            EXPAND_STEPS &expand_steps) {
    GetNode(start).Visited() = true;
    if (_IsHanging(start, ignore_created)) {
        expand_steps.emplace_back(start, -1, -1);
        return;
    }
    std::stack<NodeID> sn;
    sn.push(start);
    while (!sn.empty()) {
        auto &curr = GetNode(sn.top());
        // find the first unvisited relationship
        RelpID relp = -1;
        NodeID neighbor;
        for (auto rr : curr.RhsRelps()) {
            auto &r = GetRelationship(rr);
            auto &nbr = GetNode(r.Rhs());
            if (nbr.ID() == curr.ID()) CYPHER_TODO();  // self-loop!
            // TODO(anyone) think about this again
            if (!nbr.Visited() && (!ignore_created || (nbr.derivation_ != Node::CREATED &&
                                                       nbr.derivation_ != Node::MERGED))) {
                relp = r.ID();
                neighbor = nbr.ID();
                break;
            }
        }
        for (auto lr : curr.LhsRelps()) {
            auto &r = GetRelationship(lr);
            auto &nbr = GetNode(r.Lhs());
            if (nbr.ID() == curr.ID()) CYPHER_TODO();  // self-loop!
            if (!nbr.Visited() &&
                (!ignore_created ||
                 (nbr.derivation_ != Node::CREATED && nbr.derivation_ != Node::MERGED)) &&
                (relp < 0 || (relp >= 0 && r.ID() < relp))) {
                relp = r.ID();
                neighbor = nbr.ID();
                break;
            }
        }
        if (relp < 0) {
            sn.pop();
            continue;
        }
        expand_steps.emplace_back(curr.ID(), relp, neighbor);
        sn.push(neighbor);
        GetNode(neighbor).Visited() = true;
    }
}

Node &PatternGraph::GetNode(NodeID id) {
    // if ((size_t)id >= _nodes.size()) return EmptyNode();
    // return _nodes[id];
    for(auto &node : _nodes){
        if(node.ID() == id){
            return node;
        }
    }
    return EmptyNode();
}

Node &PatternGraph::GetNode(const std::string &alias) {
    auto it = _node_map.find(alias);
    return it == _node_map.end() ? EmptyNode() : _nodes[it->second];
}

const Node &PatternGraph::GetNode(const std::string &alias) const {
    auto it = _node_map.find(alias);
    return it == _node_map.end() ? EmptyNode() : _nodes[it->second];
}

std::vector<Node> &PatternGraph::GetNodes() { return _nodes; }

Relationship &PatternGraph::GetRelationship(RelpID id) {
    // if ((size_t)id >= _relationships.size()) return EmptyRelationship();
    // return _relationships[id];
    for(auto &relp : _relationships){
        if(relp.ID() == id){
            return relp;
        }
    }
    return EmptyRelationship();
}

const Relationship &PatternGraph::GetRelationship(RelpID id) const {
    // if ((size_t)id >= _relationships.size()) return EmptyRelationship();
    // return _relationships[id];
    for(auto &relp : _relationships){
        if(relp.ID() == id){
            return relp;
        }
    }
    return EmptyRelationship();
}

Relationship &PatternGraph::GetRelationship(const std::string &alias) {
    auto it = _relp_map.find(alias);
    return it == _relp_map.end() ? EmptyRelationship() : _relationships[it->second];
}

const Relationship &PatternGraph::GetRelationship(const std::string &alias) const {
    auto it = _relp_map.find(alias);
    return it == _relp_map.end() ? EmptyRelationship() : _relationships[it->second];
}

NodeID PatternGraph::AddNode(const std::string &label, const std::string &alias,
                             Node::Derivation derivation) {
    NodeID nid = _next_nid;
    // bool is_referenced=false;
    // if(symbol_table.symbols.find(alias) != symbol_table.symbols.end()){
    //     is_referenced=symbol_table.symbols[alias].is_referenced;
    //     // symbol_table.symbols[alias] = node;
    // }
    // _nodes.emplace_back(nid, label, alias, derivation,is_referenced);
    _nodes.emplace_back(nid, label, alias, derivation);
    _node_map.emplace(alias, nid);
    _next_nid++;
    return nid;
}

NodeID PatternGraph::AddNode(const std::string &label, const std::string &alias,
                             const Property &prop_filter, Node::Derivation derivation) {
    NodeID nid = _next_nid;
    // bool is_referenced=false;
    // if(symbol_table.symbols.find(alias) != symbol_table.symbols.end()){
    //     is_referenced=symbol_table.symbols[alias].is_referenced;
    //     // symbol_table.symbols[alias] = node;
    // }
    // _nodes.emplace_back(nid, label, alias, prop_filter, derivation,is_referenced);
    _nodes.emplace_back(nid, label, alias, prop_filter, derivation);
    _node_map.emplace(alias, nid);
    _next_nid++;
    return nid;
}

bool PatternGraph::RemoveNode(NodeID node_id) {
    // 图中的维护(_node_map,_nodes,symbol_table)
    // 相邻边全部删除
    for(size_t i=0;i<_nodes.size();i++){
        if(_nodes[i].ID()==node_id){
            auto &node=_nodes[i];
            _node_map.erase(node.Alias());
            symbol_table.symbols.erase(node.Alias());
            
            for(auto &relp_id:node.LhsRelps()){
                RemoveRelationship(relp_id);
            }
            for(auto &relp_id:node.RhsRelps()){
                RemoveRelationship(relp_id);
            }
            // node.Alias()
            _nodes.erase(_nodes.begin()+i);
            return true;
        }
    }
    // _nodes.erase(std::remove_if(_nodes.begin(), _nodes.end(),
    //                         [node_id](const Node& node) { return node.ID() == node_id; }),
    //          _nodes.end());
    return false;
}

RelpID PatternGraph::AddRelationship(const std::set<std::string> &types, NodeID lhs, NodeID rhs,
                                     parser::LinkDirection direction, const std::string &alias,
                                     Relationship::Derivation derivation) {
    return AddRelationship(types, lhs, rhs, direction, alias, -1, -1, derivation);
}

RelpID PatternGraph::AddRelationship(const std::set<std::string> &types, NodeID lhs, NodeID rhs,
                                     parser::LinkDirection direction, const std::string &alias,
                                     int min_hop, int max_hop,
                                     Relationship::Derivation derivation) {
    return AddRelationship(types, lhs, rhs, direction, alias, min_hop, max_hop, derivation, false);
}

RelpID PatternGraph::AddRelationship(const std::set<std::string> &types, NodeID lhs, NodeID rhs,
                                     parser::LinkDirection direction, const std::string &alias,
                                     int min_hop, int max_hop,
                                     Relationship::Derivation derivation, bool no_duplicate_edge) {
    RelpID rid = _next_rid;
    _relationships.emplace_back(rid, types, lhs, rhs, direction, alias, min_hop, max_hop,
                                derivation, no_duplicate_edge);
    _relp_map.emplace(alias, rid);
    _next_rid++;
    auto r = GetNode(lhs).AddRelp(rid, true);
    CYPHER_THROW_ASSERT(r);
    r = GetNode(rhs).AddRelp(rid, false);
    CYPHER_THROW_ASSERT(r);
    return rid;
}

bool PatternGraph::RemoveRelationship(RelpID relp_id){
    // 图中的维护(_relp_map,_relationships,symbol_table)
    // 源点，终点的相邻边集合维护
    for(size_t i=0;i<_relationships.size();i++){
        if(_relationships[i].ID()==relp_id){
            auto &relp=_relationships[i];
            _relp_map.erase(relp.Alias());
            symbol_table.symbols.erase(relp.Alias());
            
            auto &lhs=GetNode(relp.Lhs());
            if(!lhs.Empty()){
                lhs.DeleteRelp(relp_id,true);
            }
            auto &rhs=GetNode(relp.Rhs());
            if(!rhs.Empty()){
                rhs.DeleteRelp(relp_id,false);
            }
            // relp.Src()
            // node.Alias()
            _relationships.erase(_relationships.begin()+i);
            
            return true;
        }
    }
    return false;
}

static void ExtractNodePattern(const parser::TUP_NODE_PATTERN &node_pattern, std::string &label) {
    auto &node_labels = std::get<1>(node_pattern);
    if (!node_labels.empty()) label = node_labels[0];
}

static void ExtractNodePattern(const parser::TUP_NODE_PATTERN &node_pattern, std::string &label,
                               cypher::Property &prop) {
    ExtractNodePattern(node_pattern, label);
    auto &properties = std::get<2>(node_pattern);
    auto &expr_prop = std::get<0>(properties);
    if (expr_prop.type == parser::Expression::MAP) {
        if (expr_prop.Map().size() > 1) CYPHER_TODO();
        for (auto &m : expr_prop.Map()) {
            prop.field = m.first;
            if (m.second.type == parser::Expression::PARAMETER) {
                prop.type = Property::PARAMETER;
                prop.value_alias = m.second.String();
            } else if (m.second.type == parser::Expression::VARIABLE) {
                prop.type = Property::VARIABLE;
                prop.value_alias = m.second.String();
            } else {
                prop.type = Property::VALUE;
                prop.value = MakeFieldData(m.second);
                if (prop.value.is_null()) {
                    THROW_CODE(InputError, "Invalid expression: " + m.second.ToString());
                }
            }
        }
    }
}

NodeID PatternGraph::BuildNode(const parser::TUP_NODE_PATTERN &node_pattern,
                               Node::Derivation derivation) {
    const std::string &node_variable = std::get<0>(node_pattern);
    auto &node = GetNode(node_variable);
    std::string label;
    Property prop;
    if (node.Empty()) {
        if (derivation == Node::CREATED) {
            ExtractNodePattern(node_pattern, label);
            return AddNode(label, node_variable, derivation);
        } else {
            ExtractNodePattern(node_pattern, label, prop);
            return AddNode(label, node_variable, prop, derivation);
        }
    } else {
        // if the node already exists, update the node
        ExtractNodePattern(node_pattern, label, prop);
        node.Set(label, prop);
        return node.ID();
    }
}

RelpID PatternGraph::BuildRelationship(const parser::TUP_RELATIONSHIP_PATTERN &relp_pattern,
                                       NodeID lhs, NodeID rhs,
                                       Relationship::Derivation derivation) {
    auto direction = std::get<0>(relp_pattern);
    auto &relp_var = std::get<0>(std::get<1>(relp_pattern));
    auto &relp_detail = std::get<1>(relp_pattern);
    auto &relp_types = std::get<1>(relp_detail);
    auto &range = std::get<2>(relp_detail);
    auto &no_duplicate_edge = std::get<4>(relp_detail);
    // convert vector to set
    std::set<std::string> r_types(relp_types.begin(), relp_types.end());
    auto &relp = GetRelationship(relp_var);
    if(symbol_table.symbols.find(relp_var) == symbol_table.symbols.end()){
        size_t id=_next_rid;
        SymbolNode node=SymbolNode(id,SymbolNode::Type::RELATIONSHIP,SymbolNode::Scope::LOCAL);
        symbol_table.symbols.emplace(relp_var,node);
        // symbol_table.symbols[relp_var] = node;
    }
    if (!relp.Empty()) CYPHER_TODO();
    return AddRelationship(r_types, lhs, rhs, direction, relp_var, range[0], range[1], derivation, no_duplicate_edge);
}

std::string PatternGraph::DumpGraph() const {
    // LOG_DEBUG()<<"Dump start";
    std::string line = "Current Pattern Graph:\n";
    // LOG_DEBUG()<<"node size:";
    // LOG_DEBUG()<<_nodes.size();
    for (auto &n : _nodes) {
        // LOG_DEBUG()<<"node dump:"<<n.Alias();
        auto derivation = n.derivation_ == Node::CREATED    ? "(CREATED)"
                          : n.derivation_ == Node::MERGED   ? "(MERGED)"
                          : n.derivation_ == Node::ARGUMENT ? "(ARGUMENT)"
                                                            : "(MATCHED)";
        bool is_referenced=false;
        if(symbol_table.symbols.find(n.Alias()) != symbol_table.symbols.end()){
            is_referenced=symbol_table.symbols.at(n.Alias()).is_referenced;
        }
        line.append(fma_common::StringFormatter::Format("N[{}] {}:{} {} {}\n", std::to_string(n.ID()),
                                                        n.Alias(), n.Label(), derivation,is_referenced));
        // LOG_DEBUG()<<"node dump end";
    }
    // LOG_DEBUG()<<"node end";
    for (auto &r : _relationships) {
        auto direction = r.direction_ == parser::LEFT_TO_RIGHT   ? "-->"
                         : r.direction_ == parser::RIGHT_TO_LEFT ? "<--"
                                                                 : "--";
        auto types = fma_common::ToString(r.Types());
        auto derivation = r.derivation_ == Relationship::CREATED    ? "(CREATED)"
                          : r.derivation_ == Relationship::MERGED   ? "(MERGED)"
                          : r.derivation_ == Relationship::ARGUMENT ? "(ARGUMENT)"
                                                                    : "(MATCHED)";
        bool is_referenced=false;
        if(symbol_table.symbols.find(r.Alias()) != symbol_table.symbols.end()){
            is_referenced=symbol_table.symbols.at(r.Alias()).is_referenced;
        }
        line.append(fma_common::StringFormatter::Format(
            "R[{} {} {}] {}:{} {} {}\n", std::to_string(r.Lhs()), direction, std::to_string(r.Rhs()),
            r.Alias(), types, derivation,is_referenced));
    }
    // LOG_DEBUG()<<"relp end";
    for (auto &[symbol, symbol_node] : symbol_table.symbols) {
        line.append(fma_common::StringFormatter::Format(
            "Symbol: [{}] type({}), scope({}), symbol_id({})\n", symbol,
            cypher::SymbolNode::to_string(symbol_node.type),
            cypher::SymbolNode::to_string(symbol_node.scope), symbol_node.id));
    }
    // LOG_DEBUG()<<"symbol end";
    if (_nodes.empty() && _relationships.empty()) line.append("(EMPTY GRAPH)\n");
    // LOG_DEBUG()<<"Dump end";
    return line;
}

}  // namespace cypher
