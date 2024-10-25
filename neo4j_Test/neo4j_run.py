from neo4j import GraphDatabase
import json
from TuGraphClient import TuGraphClient
import time

neo4j_url1="bolt://localhost:7690" 
neo4j_user1="neo4j"
neo4j_password1="123456"
neo4j_url2="bolt://localhost:7691" 
neo4j_user2="neo4j"
neo4j_password2="352541141"
tugraph_url = '127.0.0.1:7073'
tugraph_user = 'admin'
tugraph_password = '73@TuGraph'
tugraph_graph = 'ldbcSf1'

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
        if(delete_Start !=-1):
            match_cypher=cypher[:delete_Start]
            varibale_cypher=cypher[delete_Start+len("delete"):].strip()
            #print("vars",varibale_cypher)
            varss=varibale_cypher.split(",")
            first_var=varss[0]
            first_start=cypher.find(first_var)-1
            if(cypher[first_start]=="["):
                is_vetex=False
            else:
                is_vetex=True
            print("vars",varss)
            with_cypher="WITH ["+",".join(varss)+"] as deleviews "
            new_cypher=match_cypher+with_cypher
            if(is_vetex):
                new_cypher+=" UNWIND deleviews as viewvetex "
                new_cypher+=" with viewvetex ".join(self.dele_node)
                
            else:
                new_cypher+=" UNWIND deleviews as viewedge "
                new_cypher+=" with viewedge ".join(self.dele_rela)
            new_cypher+=" with viewedge "+cypher
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
                new_cypher+=" with viewedge ".join(self.add_rela)
                return new_cypher
        return cypher
def neo4j_test_beforeopt(url,user,password):
    connector1=connector(url,user,password)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    path="/home/wxd/neo4j_test/graph-views/view_maintance_2024/build/output.json"
    my_tool=tool(path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    with open("./ldbcSf1/ReadQueries/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        cyphers_info={}
        for line in lines:
            line=line.rstrip("\n")
            #print(line)
            cypher_info={}
            new_line=my_tool.tran(line)
            print(new_line)
            with open("./new_cypher.txt",'w') as newfile:
                newfile.write(new_line)
            start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            result=str(result1)
            cypher_info["result"]=result
            end_time=time.time()
            execute_time=end_time-start_time
            cypher_info["time"]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cyphers_info[line]=cypher_info
        with open("./oldtime.json",'w') as timefile:
            json.dump(cyphers_info,timefile,indent=4)
def neo4j_test_afteropt(neo4j_url,neo4j_user,neo4j_password,tu_url,tu_user,tu_password,tu_db):
    connector1=connector(neo4j_url,neo4j_user,neo4j_password)    
    client = TuGraphClient(tu_url,tu_user,tu_password,tu_db)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    path="/home/wxd/neo4j_test/graph-views/view_maintance_2024/build/output.json"
    my_tool=tool(path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    with open("./ldbcSf1/ReadQueries/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        cyphers_info={}
        for line in lines:
            line=line.rstrip("\n")
            line="Optimize "+line
            print(line)
            opt_cypher_list = client.call_cypher(line)['result']
            opt_cypher=listtostr(opt_cypher_list)
            cypher_info={}
            new_line=my_tool.tran(opt_cypher)
            print(new_line)
            start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            end_time=time.time()
            result=str(result1)
            cypher_info["result"]=result
            execute_time=end_time-start_time
            cypher_info["time"]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cyphers_info[opt_cypher]=cypher_info
        with open("./opttime.json",'w') as timefile:
            json.dump(cyphers_info,timefile,indent=4)
if __name__=="__main__":
    neo4j_test_beforeopt(neo4j_url1,neo4j_user1,neo4j_password1)
    #neo4j_test_afteropt(neo4j_url2,neo4j_user2,neo4j_password2,tugraph_url,tugraph_user,tugraph_password,tugraph_graph)