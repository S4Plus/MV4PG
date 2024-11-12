from neo4j import GraphDatabase
import json
from TuGraphClient import TuGraphClient
import time
import threading
import argparse
neo4j_url1="bolt://localhost:7690" 
neo4j_user1="neo4j"
neo4j_password1="123456"
neo4j_url2="bolt://localhost:7691" 
neo4j_user2="neo4j"
neo4j_password2="352541141"
tugraph_url = '127.0.0.1:7073'
tugraph_user = 'admin'
tugraph_password = '73@TuGraph'
tugraph_graph = 'finbenchsf10'
path="/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/finbench"
def parse_args():
    parser = argparse.ArgumentParser(description="Tugraph optimization in neo4j")
    parser.add_argument('-path', '--path', help='url for tugraph')
    args = parser.parse_args()
    if args.path:
        global path
        path=args.path

def listtostr(listcypher):
    result=""
    for its in listcypher:
        for it in its: 
            result+=it
    return result
class connector:
    def __init__(self,url,user,password):
        self.driver=GraphDatabase.driver(url,auth=(user,password))
    def close(self):
        self.driver.close()
    def run_cypher(self, query, parameters=None):
        with self.driver.session() as session:
            result = session.run(query, parameters)
            # 提取所有记录到列表
            records = [record for record in result]
            # 获取查询的 summary
            summary = result.consume()
            return records, summary

class tool:
    def __init__(self,path):
        self.path=path
        with open(self.path) as self.file:
            self.triggers:dict[str,list[list[str]]]=json.load(self.file)
        self.dele_node=[]
        self.add_rela=[]
        self.dele_rela=[]
        self.views=[]
        for index, (key, value) in enumerate(self.triggers.items()):
            self.views.append(key)
            for index1, lists in enumerate(value):
                if index1 == 0:
                    for index2, list1 in enumerate(lists):
                        self.dele_node.append(list1)
                elif index1 == 1:
                    for index2, list1 in enumerate(lists):
                        self.add_rela.append(list1)
                elif index1 == 2:
                    for index2, list1 in enumerate(lists):
                        self.dele_rela.append(list1)
    def tran(self,cypher:str):
        is_vetex=False
        delete_Start=cypher.find("delete")
        create_Start=cypher.find("create")
        detach_Start=cypher.find("detach")
        if(delete_Start !=-1):
            match_cypher=""
            if detach_Start !=-1:
                match_cypher=cypher[:detach_Start]
                is_vetex=True 
            else:
                is_vetex=False               
                match_cypher=cypher[:delete_Start]
            varibale_cypher=cypher[delete_Start+len("delete"):].strip()
            #print("vars",varibale_cypher)
            varss=varibale_cypher.split(",")
            print(is_vetex)
            print("vars",varss)
            with_cypher="WITH ["+",".join(varss)+"] as deleviews "
            new_cypher=match_cypher+with_cypher
            if(is_vetex):
                new_cypher+=" UNWIND deleviews as viewvetex "
                new_cypher+=" with distinct viewvetex ".join(self.dele_node)
                new_cypher+=" with distinct viewvetex "+cypher
            else:
                new_cypher+=" UNWIND deleviews as viewedge "
                new_cypher+=" with distinct viewedge ".join(self.dele_rela)
                new_cypher+=" with distinct viewedge "+cypher
            return new_cypher
        if create_Start !=-1:
            edges=[]
            create_cypher=cypher[create_Start:]
            #print(create_cypher)
            for index,it in enumerate(create_cypher):
                if it=="[":
                    i=index
                    while(i<len(create_cypher)):
                        if create_cypher[i]==":" or create_cypher[i]=="]":
                            break
                        i+=1
                    edgevar=create_cypher[index+1:i]
                    edges.append(edgevar)
            if len(edges)==0:
                new_cypher=cypher
                return new_cypher
            else:
                edge_cypher="["+",".join(edges)+"]"
                with_cypher=" WITH "+edge_cypher+" as "+"createviews "
                new_cypher=cypher+with_cypher
                new_cypher+=" UNWIND createviews as viewedge "
                new_cypher+=" with distinct viewedge ".join(self.add_rela)
                return new_cypher
        return cypher

### init()
def create_views(path,url,user,password):
    connector1=connector(url,user,password)
    createview_time={}
    with open(path+"/views/all.txt") as createview:
        lines=createview.readlines()
        for line in lines:
            print(line)
            excute_time={}
            result1,summary=connector1.run_cypher(line)
            excute_time["result_available_after"]=summary.result_available_after
            excute_time["result_consumed_after"]=summary.result_consumed_after
            excute_time["time"]=summary.result_available_after+summary.result_consumed_after
            createview_time[line]=excute_time
    with open(path+"/result/views/createviews.json",'w') as inputfile:
        json.dump(createview_time,inputfile,indent=4)
def create_indexs(path,url,user,password):
    connector1=connector(url,user,password)
    with open(path+"/index/all.txt") as createindexs:
        lines=createindexs.readlines()
        for line in lines:
            print(line)
            connector1.run_cypher(line)
if __name__=="__main__":
    parse_args()
    try:
        create_indexs(path,neo4j_url1,neo4j_user1,neo4j_password1)
    except:
        print("error")
    try:
        create_indexs(path,neo4j_url2,neo4j_user2,neo4j_password2)
    except:
        print("exit")
    create_views(path,neo4j_url2,neo4j_user2,neo4j_password2)    
