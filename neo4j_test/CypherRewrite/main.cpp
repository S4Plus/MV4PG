#include "nlohmann/json.hpp"
#include "./neo4jcypher.h"
//#include"view_maintenance_visitor copy.h"

#include "antlr4-runtime.h"
#include "generated/LcypherLexer.h"
#include "generated/LcypherParser.h"
#include<fstream>
#include<vector>
#include<iostream>
#include<unordered_map>

using namespace parser;
using namespace antlr4;
using json = nlohmann::json;
#define NODE_INF std::tuple<std::string,std::string,std::string,bool>
std::ofstream ofs("test_out.txt",std::ios::app);
std::unordered_map<std::string,std::vector<std::vector<std::string>>> result;
void RewriteForVertex(std::string view_query,std::string node_label,std::string primary_key,std::string primary_value,bool value_is_string,bool is_create){
    ANTLRInputStream input(view_query);
    LcypherLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    // std::cout <<"parser s1"<<std::endl; // de
    LcypherParser parser(&tokens);
    ViewMaintenance visitor(parser.oC_Cypher(),node_label,primary_key,primary_value,value_is_string,is_create);
        auto querys=visitor.GetRewriteQueries();
        result[view_query].push_back(querys);
    }

void RewriteForEdge(std::string view_query,std::string edge_label,int edge_id,NODE_INF src,NODE_INF dst,bool is_create){
    ANTLRInputStream input(view_query);
    LcypherLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    // std::cout <<"parser s1"<<std::endl; // de
    LcypherParser parser(&tokens);
    ViewMaintenance visitor(parser.oC_Cypher(),edge_label,edge_id,src,dst,is_create);
    std::vector<std::string> queries=visitor.GetRewriteQueries();
    auto querys=visitor.GetRewriteQueries();
    result[view_query].push_back(querys);
}

void get_map(std::string script){
    NODE_INF src("person","id","1",false);
    NODE_INF dst("movie","id","2",false);
    RewriteForVertex(script,"keyword","id","1",false,false);
    std::cout<<"vetex ok"<<std::endl;
    RewriteForEdge(script,"write",1,src,dst,true);
    RewriteForEdge(script,"write",1,src,dst,false);
}
int main(int argv,char*argc[]){

    //std::string script="match (n)-[*2..4]->(t)-[]->(m) WITH n,m CREATE (n)-[r:test{is_view:true}]->(m)";
    // std::string script;
    // std::cin>>script;
    std::string path=argc[1];
    std::string script;
    std::string path1=path+"/views/all.txt";
    std::ifstream ifs(path1);
    while(getline(ifs,script))
    {
     std::cout<<script<<std::endl;
     get_map(script);
    }
    json j=result;
    std::string path2=path+"/output.json";
    std::ofstream outputFile(path2);
    outputFile << j.dump(4);  // 美化输出
    
}