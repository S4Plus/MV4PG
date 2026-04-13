// #include "cypher/graph/graph.h"
// #include "parser/data_typedef.h"


// namespace cypher{
//     class ViewRewriter {
//     private:
//         std::string view_name_;
//         PatternGraph *view_graph_;
//         PatternGraph *target_graph_;
//         // std::vector<bool> view_matched;
//         // std::vector<bool> target_matched;
//         std::map<NodeID,NodeID> view_to_target;
//         std::map<NodeID,NodeID> target_to_view;
//         NodeID start_node_id,end_node_id;

//         int rewrite_index=0; //对target_graph重写的次数
//     public:
//         ViewRewriter() = default;
//         ViewRewriter(PatternGraph *target_graph,PatternGraph *view_graph,std::string view_name)
//             {
//                 view_graph_=view_graph;
//                 target_graph_=target_graph;
//                 view_name_=view_name;
//                 LOG_DEBUG()<<"view graph size:"<<view_graph_->GetNodes().size();
//                 LOG_DEBUG()<<"target graph size:"<<target_graph_->GetNodes().size();
//                 LOG_DEBUG()<<"view to target size:"<<view_to_target.size();
//                 LOG_DEBUG()<<"target to view size:"<<target_to_view.size();
//                 // view_matched.resize(view_graph_->GetNodes().size(),false);
//                 // target_matched.resize(target_graph_->GetNodes().size(),false);
//                 for(auto &node:view_graph_->GetNodes()){
//                     if(node.LhsDegree()==0)start_node_id=node.ID();
//                     else if(node.RhsDegree()==0)end_node_id=node.ID();
//                 }
//             }
//         ~ViewRewriter() = default;

        

//         // 检查节点 u 和 v 是否可以匹配
//         bool IsFeasible(int view_node_id,int target_node_id) {
//             LOG_DEBUG()<<"match feasible start:"<<"view_node_id: "<<view_node_id<<",target node id:"<<target_node_id;
//             // 在这里添加你的可行性检查代码
//             auto view_node=&(view_graph_->GetNode(view_node_id));
//             auto target_node=&(target_graph_->GetNode(target_node_id));
//             LOG_DEBUG()<<"match feasible start2:"<<"view_node_name: "<<view_node->Alias()<<",target node name:"<<target_node->Alias();
//             // 非起始节点和终止节点会被删除，所以需要保证出度和入度均和视图中一致，没有别的邻点
//             // 视图中间的点出度和入度均为1
//             if(view_node_id!=start_node_id&&view_node_id!=end_node_id){
//                 if(target_node->LhsDegree()!=1||target_node->RhsDegree()!=1){
//                     LOG_DEBUG()<<"match feasible end1";
//                     return false;
//                 }
//             }
//             //相邻边的标签属性一致
//             //TODO:对于起点和终点的匹配，需要确定是哪条边匹配
//             if(view_node_id!=end_node_id){
//                 if(target_node->RhsDegree()==1){
//                     if(view_node->RhsDegree()<1)return false;
//                     auto &target_rhs_relp=target_graph_->GetRelationship(target_node->RhsRelps()[0]);
//                     if(target_rhs_relp.Empty())return false;
//                     auto &view_rhs_relp=view_graph_->GetRelationship(view_node->RhsRelps()[0]);
//                     LOG_DEBUG()<<"target rhs types:"<<target_rhs_relp.Types().size();
//                     LOG_DEBUG()<<"view rhs types:"<<view_rhs_relp.Types().size();
//                     if(target_rhs_relp.Types()!=view_rhs_relp.Types()){
//                         LOG_DEBUG()<<"match feasible end2";
//                         return false;
//                     }
//                     if(target_rhs_relp.min_hop_!=view_rhs_relp.min_hop_ || target_rhs_relp.max_hop_!=view_rhs_relp.max_hop_){
//                         LOG_DEBUG()<<"match feasible end3";
//                         return false;
//                     }
//                     //TODO:属性一致
//                     if(target_rhs_relp.no_duplicate_edge_!=view_rhs_relp.no_duplicate_edge_){
//                         LOG_DEBUG()<<"match feasible end4";
//                         return false;
//                     }
//                 }
//             }

//             if(view_node_id!=start_node_id){
//                 if(target_node->LhsDegree()==1){
//                     if(view_node->LhsDegree()<1)return false;
//                     auto &target_lhs_relp=target_graph_->GetRelationship(target_node->LhsRelps()[0]);
//                     if(target_lhs_relp.Empty())return false;
//                     auto &view_lhs_relp=view_graph_->GetRelationship(view_node->LhsRelps()[0]);
//                     if(target_lhs_relp.Types()!=view_lhs_relp.Types()){
//                         LOG_DEBUG()<<"match feasible end5";
//                         return false;
//                     }
//                     if(target_lhs_relp.min_hop_!=view_lhs_relp.min_hop_ || target_lhs_relp.max_hop_!=view_lhs_relp.max_hop_){
//                         LOG_DEBUG()<<"match feasible end3";
//                         return false;
//                     }
//                     //TODO:属性一致
//                     if(target_lhs_relp.no_duplicate_edge_!=view_lhs_relp.no_duplicate_edge_){
//                         LOG_DEBUG()<<"match feasible end6";
//                         return false;
//                     }
//                 }
//             }

//             // 点的标签属性一致
//             if(view_node->Label()!=target_node->Label()){
//                 LOG_DEBUG()<<"match feasible end7";
//                 return false;
//             }
//             //TODO:点属性一致
//             // if(view_node->Prop()!=target_node->Prop()){
//             //     return false;
//             // }
//             LOG_DEBUG()<<"match feasible end8";
//             return true;
//         }

//         // VF2 算法
//         bool VF2(NodeID view_node_id) {
//             LOG_DEBUG()<<"VF2 start";
//             // 如果匹配已经完成，返回 true
//             if (view_to_target.size() == view_graph_->GetNodes().size()) {
//                 return true;
//             }

//             // 遍历 view_graph_ 和 target_graph_ 的所有节点
//             for (auto &view_node:view_graph_->GetNodes()) {
//                 if(view_to_target.find(view_node.ID())!=view_to_target.end()){
//                     continue;
//                 }
//                 for (auto &target_node:target_graph_->GetNodes()) {
//                     if(target_to_view.find(target_node.ID())!=target_to_view.end()){
//                         continue;
//                     }
//                     // 检查节点 u 和 v 是否可以匹配
//                     auto view_node_id=view_node.ID();
//                     auto target_node_id=target_node.ID();
//                     if (IsFeasible(view_node.ID(), target_node.ID())) {
//                         // 将节点 u 和 v 添加到匹配中
//                         view_to_target[view_node_id]=target_node_id;
//                         target_to_view[target_node_id]=view_node_id;

//                         // 递归地搜索其他的匹配
//                         if (VF2()) {
//                             return true;
//                         }

//                         // 如果匹配失败，从匹配中移除节点 u 和 v
//                         view_to_target.erase(view_node_id);
//                         target_to_view.erase(target_node_id);
//                     }
//                 }
//             }
//             LOG_DEBUG()<<"VF2 end";
//             // 如果没有找到匹配，返回 false
//             return false;
//         }

//         void RewriteGraph(){
//             // view至少两个点，当为两个点时，不用删点，更改边即可
//             if(target_to_view.size()<3){
//                 //TODO：增加边的相关匹配
//                 for(auto match:target_to_view){
//                     auto view_node_id=match.second;
//                     if(view_node_id==start_node_id){
//                         auto target_node_id=match.first;
//                         auto target_node=&(target_graph_->GetNode(target_node_id));
//                         if(target_node->RhsDegree()==1){
//                             auto &target_rhs_relp=target_graph_->GetRelationship(target_node->RhsRelps()[0]);
//                             target_graph_->RemoveRelationship(target_rhs_relp.ID());
//                         }
//                     }
//                 }
//             }
//             else{
//                 for(auto match:target_to_view){
//                     auto view_node_id=match.second;
//                     if(view_node_id==start_node_id||view_node_id==end_node_id)continue;
//                     target_graph_->RemoveNode(match.first);
//                 }
//             }
//             parser::TUP_RELATIONSHIP_PATTERN relp_pattern;
//             parser::Expression e;
//             parser::TUP_PROPERTIES relp_properties=std::make_tuple(e,std::string());
//             parser::VEC_STR labels={view_name_};
//             std::string anon_name=std::string("@ANON_")+view_name_;
//             anon_name.append(std::to_string(rewrite_index));
//             rewrite_index++;
//             parser::TUP_RELATIONSHIP_DETAIL relp_detail=std::make_tuple(anon_name,labels,std::array<int, 2>{-1,-1},relp_properties,false);
//             relp_pattern=std::make_tuple(parser::LinkDirection::LEFT_TO_RIGHT,relp_detail);
//             auto target_start_id=view_to_target[start_node_id];
//             auto target_end_id=view_to_target[end_node_id];
//             target_graph_->BuildRelationship(relp_pattern,target_start_id,target_end_id,Relationship::Derivation::MATCHED);
//             LOG_DEBUG()<<"target graph:";
//             LOG_DEBUG()<<target_graph_->DumpGraph();
//         }

//         void GraphRewriteUseViews() {
//             // std::vector<NodeID> core_1;
//             // std::vector<NodeID> core_2;
//             // view_matched.resize(view_graph_.GetNodes().size(),false);
//             // target_matched.resize(target_graph_.GetNodes().size(),false);
//             // while(VF2()){
//             while(VF2(start_node_id)){
//                 for(auto match:view_to_target){
//                     auto view_name=view_graph_->GetNode(match.first).Alias();
//                     auto target_name=target_graph_->GetNode(match.second).Alias();
//                     LOG_DEBUG()<<match.first<<":"<<view_name<<" , ";
//                     LOG_DEBUG()<<match.second<<":"<<target_name<<std::endl;
//                 }
//                 RewriteGraph();
//                 view_to_target.clear();
//                 target_to_view.clear();
//             }
//             // bool result=vf2();
//             // if(result){
//             //     RewriteGraph();
//             //     // std::cout<<core_1<<std::endl;
//             //     for(int i=0;i<core_1.size();i++){
//             //         auto view_name=view_graph_->GetNode(core_1[i]).Alias();
//             //         auto target_name=target_graph_->GetNode(core_2[i]).Alias();
//             //         std::cout<<core_1[i]<<":"<<view_name<<" , ";
//             //         std::cout<<core_2[i]<<":"<<target_name<<std::endl;
//             //     }
//             // }
//         }
//     };
// }