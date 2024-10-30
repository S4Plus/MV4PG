#include "cypher/graph/graph.h"
#include "parser/data_typedef.h"


namespace cypher{
    class ViewRewriter {
    private:
        std::string view_name_;
        PatternGraph *view_graph_;
        PatternGraph *target_graph_;
        // std::vector<bool> view_matched;
        // std::vector<bool> target_matched;
        std::map<NodeID,NodeID> view_to_target;
        std::map<NodeID,NodeID> target_to_view;
        NodeID start_node_id,end_node_id;
        NodeID now_view_id=-1,now_target_id=-1;//目前匹配到的视图节点id和对应的目标节点id

        int rewrite_index=0; //对target_graph重写的次数
    public:
        ViewRewriter() = default;
        ViewRewriter(PatternGraph *target_graph,PatternGraph *view_graph,std::string view_name)
            {
                view_graph_=view_graph;
                target_graph_=target_graph;
                view_name_=view_name;
#ifndef NDEBUG
                LOG_DEBUG()<<"view graph size:"<<view_graph_->GetNodes().size();
                LOG_DEBUG()<<"target graph size:"<<target_graph_->GetNodes().size();
                LOG_DEBUG()<<"view to target size:"<<view_to_target.size();
                LOG_DEBUG()<<"target to view size:"<<target_to_view.size();
#endif
                // view_matched.resize(view_graph_->GetNodes().size(),false);
                // target_matched.resize(target_graph_->GetNodes().size(),false);
                for(auto &node:view_graph_->GetNodes()){
                    if(node.LhsDegree()==0)start_node_id=node.ID();
                    else if(node.RhsDegree()==0)end_node_id=node.ID();
                }
                for(auto &node:view_graph_->GetNodes()){
                    if(node.Alias()=="n")start_node_id=node.ID();
                    else if(node.Alias()=="m")end_node_id=node.ID();
                }
            }
        ~ViewRewriter() = default;

        

        // 检查节点 u 和 v 是否可以匹配
        bool IsFeasible(int view_node_id,int target_node_id) {
#ifndef NDEBUG
            LOG_DEBUG()<<"match feasible start:"<<"view_node_id: "<<view_node_id<<",target node id:"<<target_node_id;
#endif
            auto view_node=&(view_graph_->GetNode(view_node_id));
            auto target_node=&(target_graph_->GetNode(target_node_id));
#ifndef NDEBUG
            LOG_DEBUG()<<"match feasible start2:"<<"view_node_name: "<<view_node->Alias()<<",target node name:"<<target_node->Alias();
#endif
            // 非起始节点和终止节点会被删除，所以需要保证出度和入度均和视图中一致，没有别的邻点
            // 视图中间的点出度和入度均为1
            if(view_node_id!=start_node_id&&view_node_id!=end_node_id){
                if(target_node->LhsDegree()!=1||target_node->RhsDegree()!=1){
#ifndef NDEBUG
                    LOG_DEBUG()<<"match feasible end1";
#endif
                    return false;
                }
            }
            //相邻边的标签属性一致
            //TODO:对于起点和终点的匹配，需要确定是哪条边匹配
            if(view_node_id!=end_node_id){
                if(target_node->RhsDegree()==1){
                    if(view_node->RhsDegree()<1)return false;
                    auto &target_rhs_relp=target_graph_->GetRelationship(target_node->RhsRelps()[0]);
                    if(target_rhs_relp.Empty())return false;
                    auto &view_rhs_relp=view_graph_->GetRelationship(view_node->RhsRelps()[0]);
#ifndef NDEBUG
                    LOG_DEBUG()<<"target rhs types:"<<target_rhs_relp.Types().size();
                    LOG_DEBUG()<<"view rhs types:"<<view_rhs_relp.Types().size();
#endif
                    if(target_rhs_relp.Types()!=view_rhs_relp.Types()){
#ifndef NDEBUG
                        LOG_DEBUG()<<"match feasible end2";
#endif
                        return false;
                    }
                    if(target_rhs_relp.min_hop_!=view_rhs_relp.min_hop_ || target_rhs_relp.max_hop_!=view_rhs_relp.max_hop_){
#ifndef NDEBUG
                        LOG_DEBUG()<<"match feasible end3";
#endif
                        return false;
                    }
                    //TODO:属性一致
                    if(target_rhs_relp.no_duplicate_edge_!=view_rhs_relp.no_duplicate_edge_){
#ifndef NDEBUG
                        LOG_DEBUG()<<"match feasible end4";
#endif
                        return false;
                    }
                }
            }

            if(view_node_id!=start_node_id){
                if(target_node->LhsDegree()==1){
                    if(view_node->LhsDegree()<1)return false;
                    auto &target_lhs_relp=target_graph_->GetRelationship(target_node->LhsRelps()[0]);
                    if(target_lhs_relp.Empty())return false;
                    auto &view_lhs_relp=view_graph_->GetRelationship(view_node->LhsRelps()[0]);
                    if(target_lhs_relp.Types()!=view_lhs_relp.Types()){
#ifndef NDEBUG
                        LOG_DEBUG()<<"match feasible end5";
#endif
                        return false;
                    }
                    if(target_lhs_relp.min_hop_!=view_lhs_relp.min_hop_ || target_lhs_relp.max_hop_!=view_lhs_relp.max_hop_){
#ifndef NDEBUG
                        LOG_DEBUG()<<"match feasible end3";
#endif
                        return false;
                    }
                    //TODO:属性一致
                    if(target_lhs_relp.no_duplicate_edge_!=view_lhs_relp.no_duplicate_edge_){
#ifndef NDEBUG
                        LOG_DEBUG()<<"match feasible end6";
#endif
                        return false;
                    }
                }
            }

            // 点的标签属性一致
            if(view_node->Label()!=target_node->Label()){
#ifndef NDEBUG
                LOG_DEBUG()<<"match feasible end7";
#endif
                return false;
            }
            //TODO:点属性一致
            // if(view_node->Prop()!=target_node->Prop()){
            //     return false;
            // }
#ifndef NDEBUG
            LOG_DEBUG()<<"match feasible end8";
#endif
            return true;
        }

        bool NodeFeasible(NodeID view_node_id,NodeID target_node_id){
            auto view_node=&(view_graph_->GetNode(view_node_id));
            auto target_node=&(target_graph_->GetNode(target_node_id));
            return NodeFeasible(view_node,target_node);
            // if(view_node->Empty()||target_node->Empty())return false;
            // // 非起始点和终点会被删除，保证这些点不和别的点相连，并且没有别的地方引用          
            // if(view_node_id!=start_node_id&&view_node_id!=end_node_id){
            //     auto target_symbols=target_graph_->symbol_table.symbols;
            //     if(target_symbols.find(target_node->Alias())!=target_symbols.end()){
            //         if(target_symbols[target_node->Alias()].is_referenced)
            //             return false;
            //     }
            //     if((target_node->LhsDegree()+target_node->RhsDegree())!=2){
            //         LOG_DEBUG()<<"match feasible end1";
            //         return false;
            //     }
            // }
            // if(view_node->Label()!=target_node->Label()){
            //     LOG_DEBUG()<<"match feasible end7";
            //     return false;
            // }
            // //TODO:点属性一致
            // // if(view_node->Prop()!=target_node->Prop()){
            // //     return false;
            // // }
            // LOG_DEBUG()<<"match feasible end8";
            // return true;
        }

        bool NodeFeasible(cypher::Node* view_node,cypher::Node* target_node){
            if(view_node->Empty()||target_node->Empty())return false;
            // 非起始点和终点会被删除，保证这些点不和别的点相连，并且没有别的地方引用          
            if(view_node->ID()!=start_node_id&&view_node->ID()!=end_node_id){
                auto target_symbols=target_graph_->symbol_table.symbols;
                if(target_symbols.find(target_node->Alias())!=target_symbols.end()){
                    if(target_symbols[target_node->Alias()].is_referenced)
                        return false;
                }
                if((target_node->LhsDegree()+target_node->RhsDegree())!=2){
#ifndef NDEBUG
                    LOG_DEBUG()<<"match feasible end1";
#endif
                    return false;
                }
                if(view_node->Label()!=target_node->Label()){
#ifndef NDEBUG
                LOG_DEBUG()<<"match feasible end9";
#endif
                return false;
                }
                if(!(view_node->Prop()==target_node->Prop())){
                    return false;
                }
            }
            else{
                //view_node没有标签意味着没有限制，target_node是什么都行
                //如果是target_node没有标签，那么有部分是符合view_node的，这个要加吗？
                if(view_node->Label().size()!=0 && view_node->Label()!=target_node->Label()){
#ifndef NDEBUG
                    LOG_DEBUG()<<"match feasible end7";
#endif
                    return false;
                }

                if(!(view_node->Prop().Empty()) && !(view_node->Prop()==target_node->Prop())){
                    return false;
                }
#ifndef NDEBUG
            LOG_DEBUG()<<"match feasible end8";
#endif
            }
            return true;
        }

        bool RelpFeasible(RelpID view_relp_id,RelpID target_relp_id,cypher::NodeID view_relp_src,cypher::NodeID target_relp_src){
            auto view_relp=&(view_graph_->GetRelationship(view_relp_id));
            auto target_relp=&(target_graph_->GetRelationship(target_relp_id));
            return RelpFeasible(view_relp,target_relp,view_relp_src,target_relp_src);
        }
        //     if(view_relp->Empty()||target_relp->Empty())return false;
        //     // 匹配的边最后要全部删除，所以需要保证没有别的地方引用
        //     auto target_symbols=target_graph_->symbol_table.symbols;
        //     LOG_DEBUG()<<"target relp alias:"<<target_relp->Alias()<<" has symbol:"<<(target_symbols.find(target_relp->Alias())!=target_symbols.end());
        //     if(target_symbols.find(target_relp->Alias())!=target_symbols.end()){
        //         LOG_DEBUG()<<"target relp alias:"<<target_relp->Alias()<<" is referenced:"<<target_symbols[target_relp->Alias()].is_referenced;
        //         if(target_symbols[target_relp->Alias()].is_referenced)
        //             return false;
        //     }
        //     if(!is_opposite_direction){
        //         if(view_relp->direction_!=target_relp->direction_){
        //             return false;
        //         }
        //     }
        //     else{
        //         if(view_relp->direction_==parser::DIR_NOT_SPECIFIED){
        //             if(target_relp->direction_!=parser::DIR_NOT_SPECIFIED){
        //                 return false;
        //             }
        //         }
        //         else if(view_relp->direction_==parser::LEFT_TO_RIGHT){
        //             if(target_relp->direction_!=parser::RIGHT_TO_LEFT){
        //                 return false;
        //             }
        //         }
        //         else if(view_relp->direction_==parser::RIGHT_TO_LEFT){
        //             if(target_relp->direction_!=parser::LEFT_TO_RIGHT){
        //                 return false;
        //             }
        //         }
        //         else return false;
        //     }
        //     if(view_relp->Types()!=target_relp->Types()){
        //         return false;
        //     }
        //     if(view_relp->min_hop_!=target_relp->min_hop_ || view_relp->max_hop_!=target_relp->max_hop_){
        //         return false;
        //     }
        //     if(view_relp->no_duplicate_edge_!=target_relp->no_duplicate_edge_){
        //         LOG_DEBUG()<<"match feasible end6";
        //         return false;
        //     }
        //     //TODO:属性一致
        //     return true;
        // }

        bool RelpFeasible(cypher::Relationship *view_relp,cypher::Relationship *target_relp,cypher::NodeID view_relp_src,cypher::NodeID target_relp_src){
            if(view_relp->Empty()||target_relp->Empty())return false;
            // 匹配的边最后要全部删除，所以需要保证没有别的地方引用
            auto target_symbols=target_graph_->symbol_table.symbols;
#ifndef NDEBUG
            LOG_DEBUG()<<"target relp alias:"<<target_relp->Alias()<<" has symbol:"<<(target_symbols.find(target_relp->Alias())!=target_symbols.end());
#endif
            if(target_symbols.find(target_relp->Alias())!=target_symbols.end()){
#ifndef NDEBUG
                LOG_DEBUG()<<"target relp alias:"<<target_relp->Alias()<<" is referenced:"<<target_symbols[target_relp->Alias()].is_referenced;
#endif
                if(target_symbols[target_relp->Alias()].is_referenced)
                    return false;
            }

            if(!((view_relp->Src()==view_relp_src && target_relp->Src()==target_relp_src)
              || (view_relp->Dst()==view_relp_src && target_relp->Dst()==target_relp_src))){
                // LOG_DEBUG()<<"view_relp_src: "<<view_graph_->GetNode(view_relp_src).Alias();
                // LOG_DEBUG()<<"target relp src: "<<target_graph_->GetNode(target_relp_src).Alias();
                return false;
            }
            // if(!is_opposite_direction){
            //     if(view_relp->direction_!=target_relp->direction_){
            //         return false;
            //     }
            // }
            // else{
            //     if(view_relp->direction_==parser::DIR_NOT_SPECIFIED){
            //         if(target_relp->direction_!=parser::DIR_NOT_SPECIFIED){
            //             return false;
            //         }
            //     }
            //     else if(view_relp->direction_==parser::LEFT_TO_RIGHT){
            //         if(target_relp->direction_!=parser::RIGHT_TO_LEFT){
            //             return false;
            //         }
            //     }
            //     else if(view_relp->direction_==parser::RIGHT_TO_LEFT){
            //         if(target_relp->direction_!=parser::LEFT_TO_RIGHT){
            //             return false;
            //         }
            //     }
            //     else return false;
            // }
            if(view_relp->Types()!=target_relp->Types()){
                // LOG_DEBUG()<<"match feasible end4";
                return false;
            }
            if(view_relp->min_hop_!=target_relp->min_hop_ || view_relp->max_hop_!=target_relp->max_hop_){
                // LOG_DEBUG()<<"match feasible end5";
                return false;
            }
            if(view_relp->no_duplicate_edge_!=target_relp->no_duplicate_edge_){
#ifndef NDEBUG
                LOG_DEBUG()<<"match feasible end6";
#endif
                return false;
            }
            //TODO:属性一致，添加边的属性
            // LOG_DEBUG()<<"relp feasible true";
            return true;
        }

        // VF2 算法
        bool VF2() {
#ifndef NDEBUG
            LOG_DEBUG()<<"VF2 start";
#endif
            // 如果匹配已经完成，返回 true
            if (view_to_target.size() == view_graph_->GetNodes().size()) {
                return true;
            }
            // auto &view_node=view_graph_->GetNode(view_node_id);
            // if(view_node.Empty())return false;
#ifndef NDEBUG
            LOG_DEBUG()<<"now view id"<<now_view_id;
            LOG_DEBUG()<<"now target id:"<<now_target_id;
#endif
            if(now_view_id==-1){
                // now_view_id=start_node_id;
                for(auto &target_node:target_graph_->GetNodes()){
                    now_view_id=start_node_id;
                    if(NodeFeasible(now_view_id,target_node.ID())){
                        // now_view_id=start_node_id;
                        int pre_view_id=now_view_id;
                        int pre_target_id=now_target_id;
                        now_target_id=target_node.ID();
                        view_to_target[now_view_id]=now_target_id;
                        target_to_view[now_target_id]=now_view_id;
                        if(VF2()){
                            return true;
                        }
                        view_to_target.erase(pre_view_id);
                        target_to_view.erase(pre_target_id);
                        now_view_id=-1;
                        now_target_id=-1;
                    }
                }
            }
            else{
                auto now_view_node=&(view_graph_->GetNode(now_view_id));
                auto now_target_node=&(target_graph_->GetNode(now_target_id));
                NodeID pre_view_id=now_view_id;
                NodeID pre_target_id=now_target_id;
                for(auto view_rhs_relp_id:now_view_node->RhsRelps()){
                    auto view_rhs_relp=&(view_graph_->GetRelationship(view_rhs_relp_id));
                    if(view_rhs_relp->Empty())continue;
                    auto neighbor_view_id=view_rhs_relp->Rhs();
                    // 已经匹配的不用再匹配
                    if(view_to_target.find(neighbor_view_id)!=view_to_target.end())continue;
                    // 遍历对应目标节点的右邻边和左邻边
                    for(auto target_rhs_relp_id:now_target_node->RhsRelps()){
                        auto target_rhs_relp=&(target_graph_->GetRelationship(target_rhs_relp_id));
                        if(target_rhs_relp->Empty())continue;
                        auto neighbor_target_id=target_rhs_relp->Rhs();
                        if(target_to_view.find(neighbor_target_id)!=target_to_view.end())continue;
                        if(RelpFeasible(view_rhs_relp,target_rhs_relp,now_view_id,now_target_id)&&NodeFeasible(neighbor_view_id,neighbor_target_id)){
                            now_view_id=neighbor_view_id;
                            now_target_id=neighbor_target_id;
                            view_to_target[neighbor_view_id]=neighbor_target_id;
                            target_to_view[neighbor_target_id]=neighbor_view_id;
                            if(VF2()){
                                return true;
                            }
                            now_view_id=pre_view_id;
                            now_target_id=pre_target_id;
                            view_to_target.erase(neighbor_view_id);
                            target_to_view.erase(neighbor_target_id);
                        }
                    }
                    for(auto target_lhs_relp_id:now_target_node->LhsRelps()){
                        auto target_lhs_relp=&(target_graph_->GetRelationship(target_lhs_relp_id));
                        if(target_lhs_relp->Empty())continue;
                        auto neighbor_target_id=target_lhs_relp->Lhs();
                        if(target_to_view.find(neighbor_target_id)!=target_to_view.end())continue;
                        if(RelpFeasible(view_rhs_relp,target_lhs_relp,now_view_id,now_target_id)&&NodeFeasible(neighbor_view_id,neighbor_target_id)){
                            now_view_id=neighbor_view_id;
                            now_target_id=neighbor_target_id;
                            view_to_target[neighbor_view_id]=neighbor_target_id;
                            target_to_view[neighbor_target_id]=neighbor_view_id;
                            if(VF2()){
                                return true;
                            }
                            //回溯
                            now_view_id=pre_view_id;
                            now_target_id=pre_target_id;
                            view_to_target.erase(neighbor_view_id);
                            target_to_view.erase(neighbor_target_id);
                        }
                    }
                }
                for(auto view_lhs_relp_id:now_view_node->LhsRelps()){
                    auto view_lhs_relp=&(view_graph_->GetRelationship(view_lhs_relp_id));
                    if(view_lhs_relp->Empty())continue;
                    auto neighbor_view_id=view_lhs_relp->Lhs();
                    if(view_to_target.find(neighbor_view_id)!=view_to_target.end())continue;
                    // 遍历对应目标节点的右邻边和左邻边
                    for(auto target_rhs_relp_id:now_target_node->RhsRelps()){
                        auto target_rhs_relp=&(target_graph_->GetRelationship(target_rhs_relp_id));
                        if(target_rhs_relp->Empty())continue;
                        auto neighbor_target_id=target_rhs_relp->Rhs();
                        if(target_to_view.find(neighbor_target_id)!=target_to_view.end())continue;
                        if(RelpFeasible(view_lhs_relp,target_rhs_relp,now_view_id,now_target_id)&&NodeFeasible(neighbor_view_id,neighbor_target_id)){
                            now_view_id=neighbor_view_id;
                            now_target_id=neighbor_target_id;
                            view_to_target[neighbor_view_id]=neighbor_target_id;
                            target_to_view[neighbor_target_id]=neighbor_view_id;
                            if(VF2()){
                                return true;
                            }
                            now_view_id=pre_view_id;
                            now_target_id=pre_target_id;
                            view_to_target.erase(neighbor_view_id);
                            target_to_view.erase(neighbor_target_id);
                        }
                    }
                    for(auto target_lhs_relp_id:now_target_node->LhsRelps()){
                        auto target_lhs_relp=&(target_graph_->GetRelationship(target_lhs_relp_id));
                        if(target_lhs_relp->Empty())continue;
                        auto neighbor_target_id=target_lhs_relp->Lhs();
                        if(target_to_view.find(neighbor_target_id)!=target_to_view.end())continue;
                        if(RelpFeasible(view_lhs_relp,target_lhs_relp,now_view_id,now_target_id)&&NodeFeasible(neighbor_view_id,neighbor_target_id)){
                            now_view_id=neighbor_view_id;
                            now_target_id=neighbor_target_id;
                            view_to_target[neighbor_view_id]=neighbor_target_id;
                            target_to_view[neighbor_target_id]=neighbor_view_id;
                            if(VF2()){
                                return true;
                            }
                            //回溯
                            now_view_id=pre_view_id;
                            now_target_id=pre_target_id;
                            view_to_target.erase(neighbor_view_id);
                            target_to_view.erase(neighbor_target_id);
                        }
                    }
                }
            }

            // 遍历 view_graph_ 和 target_graph_ 的所有节点
            // for (auto &view_node:view_graph_->GetNodes()) {
            //     if(view_to_target.find(view_node.ID())!=view_to_target.end()){
            //         continue;
            //     }
            //     for (auto &target_node:target_graph_->GetNodes()) {
            //         if(target_to_view.find(target_node.ID())!=target_to_view.end()){
            //             continue;
            //         }
            //         // 检查节点 u 和 v 是否可以匹配
            //         auto view_node_id=view_node.ID();
            //         auto target_node_id=target_node.ID();
            //         if (IsFeasible(view_node.ID(), target_node.ID())) {
            //             // 将节点 u 和 v 添加到匹配中
            //             view_to_target[view_node_id]=target_node_id;
            //             target_to_view[target_node_id]=view_node_id;

            //             // 递归地搜索其他的匹配
            //             if (VF2()) {
            //                 return true;
            //             }

            //             // 如果匹配失败，从匹配中移除节点 u 和 v
            //             view_to_target.erase(view_node_id);
            //             target_to_view.erase(target_node_id);
            //         }
            //     }
            // }
#ifndef NDEBUG
            LOG_DEBUG()<<"VF2 end";
#endif
            // 如果没有找到匹配，返回 false
            return false;
        }

        void RewriteGraph(){
            // view至少两个点，当为两个点时，不用删点，更改边即可
            for(auto pair : target_to_view){
                LOG_DEBUG()<<"target to view :"<<pair.first<<","<<pair.second;
            }
            if(view_graph_->GetNodes().size()<3){
                //TODO：增加边的相关匹配
                NodeID target_start_id=view_to_target[start_node_id];
                NodeID target_end_id=view_to_target[end_node_id];
                auto target_start_node=&(target_graph_->GetNode(target_start_id));
                bool find=false;
                for(auto relp:target_start_node->RhsRelps()){
                    auto &target_rhs_relp=target_graph_->GetRelationship(relp);
                    if(target_rhs_relp.Rhs()==target_end_id){
                        target_graph_->RemoveRelationship(target_rhs_relp.ID());
                        find=true;
                        break;
                    }
                }
                if(!find){
                    for(auto relp:target_start_node->LhsRelps()){
                        auto &target_lhs_relp=target_graph_->GetRelationship(relp);
                        if(target_lhs_relp.Lhs()==target_end_id){
                            target_graph_->RemoveRelationship(target_lhs_relp.ID());
                            find=true;
                            break;
                        }
                    }
                }
                // for(auto match:target_to_view){
                //     auto view_node_id=match.second;
                //     if(view_node_id==start_node_id){
                //         auto target_node_id=match.first;
                //         auto target_node=&(target_graph_->GetNode(target_node_id));
                //         if(target_node->RhsDegree()==1){
                //             auto &target_rhs_relp=target_graph_->GetRelationship(target_node->RhsRelps()[0]);
                //             target_graph_->RemoveRelationship(target_rhs_relp.ID());
                //         }
                //         else{
                //             auto &target_lhs_relp=target_graph_->GetRelationship(target_node->LhsRelps()[0]);
                //             target_graph_->RemoveRelationship(target_lhs_relp.ID());
                //         }
                //     }
                // }
            }
            else{
                for(auto match:target_to_view){
                    auto view_node_id=match.second;
                    if(view_node_id==start_node_id||view_node_id==end_node_id)continue;
                    target_graph_->RemoveNode(match.first);
                }
            }
            parser::TUP_RELATIONSHIP_PATTERN relp_pattern;
            parser::Expression e;
            parser::TUP_PROPERTIES relp_properties=std::make_tuple(e,std::string());
            parser::VEC_STR labels={view_name_};
            std::string anon_name=std::string("@ANON_")+view_name_;
            anon_name.append(std::to_string(rewrite_index));
            rewrite_index++;
            parser::TUP_RELATIONSHIP_DETAIL relp_detail=std::make_tuple(anon_name,labels,std::array<int, 2>{-1,-1},relp_properties,false);
            relp_pattern=std::make_tuple(parser::LinkDirection::LEFT_TO_RIGHT,relp_detail);
            auto target_start_id=view_to_target[start_node_id];
            auto target_end_id=view_to_target[end_node_id];
            target_graph_->BuildRelationship(relp_pattern,target_start_id,target_end_id,Relationship::Derivation::MATCHED);
#ifndef NDEBUG
            LOG_DEBUG()<<"target graph:";
            LOG_DEBUG()<<target_graph_->DumpGraph();
#endif
        }

        void GraphRewriteUseViews() {
            // std::vector<NodeID> core_1;
            // std::vector<NodeID> core_2;
            // view_matched.resize(view_graph_.GetNodes().size(),false);
            // target_matched.resize(target_graph_.GetNodes().size(),false);
            // while(VF2()){
            while(VF2()){
                for(auto match:view_to_target){
                    auto view_name=view_graph_->GetNode(match.first).Alias();
                    auto target_name=target_graph_->GetNode(match.second).Alias();
#ifndef NDEBUG
                    LOG_DEBUG()<<match.first<<":"<<view_name<<" , ";
                    LOG_DEBUG()<<match.second<<":"<<target_name<<std::endl;
#endif
                }
                RewriteGraph();
                view_to_target.clear();
                target_to_view.clear();
                now_view_id=-1;
                now_target_id=-1;
            }
            // bool result=vf2();
            // if(result){
            //     RewriteGraph();
            //     // std::cout<<core_1<<std::endl;
            //     for(int i=0;i<core_1.size();i++){
            //         auto view_name=view_graph_->GetNode(core_1[i]).Alias();
            //         auto target_name=target_graph_->GetNode(core_2[i]).Alias();
            //         std::cout<<core_1[i]<<":"<<view_name<<" , ";
            //         std::cout<<core_2[i]<<":"<<target_name<<std::endl;
            //     }
            // }
        }
    };
}