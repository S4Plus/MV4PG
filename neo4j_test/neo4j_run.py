from neo4j import GraphDatabase
import json
import logging
from TuGraphClient import TuGraphClient
import time
import argparse
import threading
logging.basicConfig(level=logging.ERROR)
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
path="/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/finbench"
def filter(profile):
    result={}
    db=0
    dbHits=profile.get("dbHits")
    print(dbHits)
    if dbHits is not None:
        db=dbHits
    rows=profile.get("rows")
    typee=profile.get("operatorType")
    args=profile.get("args")
    detail=args["Details"]
    new_children=[]
    children=profile.get("children")
    if children is not None:
        for child in children:
            dbs,pa=filter(child)
            db+=dbs
            new_children.append(pa)
    result["operatorType"]=typee
    result["Details"]=detail
    result["dbHits"]=dbHits
    result["rows"]=rows
    result["children"]=new_children 
    return db,result
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
def parse_args():
    parser = argparse.ArgumentParser(description="Tugraph optimization in neo4j")
    parser.add_argument('-path', '--path', help='url for tugraph')
    parser.add_argument('-tugraph', '--tugraph', help='url for tugraph')
    args = parser.parse_args()
    if args.path:
        global path
        path=args.path
    if args.tugraph:
        global tugraph_graph
        tugraph_graph=args.tugraph
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
            #print(is_vetex)
            #print("vars",varss)
            with_cypher="WITH ["+",".join(varss)+"] as delete_views "
            new_cypher=match_cypher+with_cypher
            if(is_vetex):
                new_cypher+=" UNWIND delete_views as view_vetex "
                new_cypher+=" with view_vetexs UNWIND view_vetexs as view_vetex with DISTINCT view_vetex ".join(self.dele_node)
                new_cypher+=" with DISTINCT view_vetexs "+cypher
            else:
                new_cypher+=" UNWIND delete_views as view_edge "
                new_cypher+=" with view_edges UNWIND view_edges as view_edge with DISTINCT view_edge ".join(self.dele_rela)
                new_cypher+=" with DISTINCT view_edges "+cypher
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
                with_cypher=" WITH "+edge_cypher+" as "+"create_views "
                new_cypher=cypher+with_cypher
                new_cypher+=" UNWIND create_views as view_edge "
                new_cypher+=" with DISTINCT view_edge ".join(self.add_rela)
                return new_cypher
        return cypher

def neo4j_test_beforeopt(path,url,user,password):
    connector1=connector(url,user,password)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    output_path=path+"/output.json"
    my_tool=tool(output_path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    with open(path+"/ReadQueries/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        cyphers_info={}
        for line in lines:
            line=line.rstrip("\n")
            #print(line)
            cypher_info={}
            new_line=my_tool.tran(line)
            new_line="profile "+new_line
            print(new_line)
            alltime=0.0
            for number in range(5):
                #start_time=time.time()
                result1,summary=connector1.run_cypher(new_line)
                result=str(result1)
                cypher_info["result"+str(number)]=result
                #end_time=time.time()
                #execute_time=end_time-start_time
                execute_time=summary.result_available_after+summary.result_consumed_after
                alltime+=execute_time
                cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)
                cypher_info["time"+str(number)]=execute_time
                cypher_info["add_nodes"]=summary.counters.nodes_created
                cypher_info["delete_nodes"]=summary.counters.nodes_deleted
                cypher_info["add_rel"]=summary.counters.relationships_created
                cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cypher_info["alltime"]=alltime
            cypher_info["ave_time"]=alltime/5.0
            cyphers_info[line]=cypher_info
        with open(path+"/result/oldtime.json",'w') as timefile:
            json.dump(cyphers_info,timefile,indent=4)


def neo4j_test_writebeforeopt(path,url,user,password):
    connector1=connector(url,user,password)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    output_path=path+"/output.json"
    my_tool=tool(output_path)
    time1=[]
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    with open(path+"/WriteQueries/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        cyphers_info={}
        for line in lines:
            line=line.rstrip("\n")
            #print(line)
            cypher_info={}
            new_line=line
            print(new_line)
            #start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            result=str(result1)
            cypher_info["result"]=result
            #end_time=time.time()
            #execute_time=end_time-start_time
            execute_time=summary.result_available_after+summary.result_consumed_after
            time1.append(execute_time)
            cypher_info["time"]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cyphers_info[line]=cypher_info
        with open(path+"/result/oldwritetime.json",'w') as timefile:
            json.dump(cyphers_info,timefile,indent=4)
    return time1


def neo4j_test_coverbeforeopt(path,url,user,password):
    connector1=connector(url,user,password)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    output_path=path+"/output.json"
    my_tool=tool(output_path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    with open(path+"/recover/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        for line in lines:
            line=line.rstrip("\n")
            #print(line)
            new_line=line
            print(new_line)
            connector1.run_cypher(new_line)

#opt
def neo4j_test_afteropt(path,neo4j_url,neo4j_user,neo4j_password):
    connector1=connector(neo4j_url,neo4j_user,neo4j_password)
    client = TuGraphClient(tugraph_url,tugraph_user,tugraph_password,tugraph_graph)    
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    output_path=path+"/output.json"
    my_tool=tool(output_path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    with open(path+"/ReadQueries/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        cyphers_info={}
        for line in lines:
            line=line.rstrip("\n")
            line="Optimize "+line
            print(line)
            opt_start_time=time.time()
            opt_cypher_list = client.call_cypher(line)['result']
            opt_end_time=time.time()
            opt_cypher=listtostr(opt_cypher_list)
            cypher_info={}
            opt_time=opt_end_time-opt_start_time
            cypher_info["opt_time"]=opt_time
            new_line=my_tool.tran(opt_cypher)
            new_line="profile "+new_line
            print(new_line)
            alltime=0.0
            for number in range(5):
                #start_time=time.time()
                result1,summary=connector1.run_cypher(new_line)
                result=str(result1)
                cypher_info["result"+str(number)]=result
                #end_time=time.time()
                #execute_time=end_time-start_time
                execute_time=summary.result_available_after+summary.result_consumed_after
                alltime+=execute_time
                cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)
                cypher_info["time"+str(number)]=execute_time
                cypher_info["add_nodes"]=summary.counters.nodes_created
                cypher_info["delete_nodes"]=summary.counters.nodes_deleted
                cypher_info["add_rel"]=summary.counters.relationships_created
                cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cypher_info["alltime"]=alltime
            cypher_info["ave_time"]=alltime/5.0
            cyphers_info[new_line]=cypher_info
        with open(path+"/result/opttime.json",'w') as timefile:
            json.dump(cyphers_info,timefile,indent=4)


def neo4j_test_writeafteropt(path,url,user,password):
    connector1=connector(url,user,password)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    output_path=path+"/output.json"
    my_tool=tool(output_path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    time1=[]
    cyphers_info={}
    with open(path+"/WriteQueries/all.txt",'r') as inputfile:
        lines=inputfile.readlines()    
        for line in lines:
            line=line.rstrip("\n")
            print(line)
            #print(line)
            cypher_info={}
            new_line=my_tool.tran(line)
            #print(new_line)
            #start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            result=str(result1)
            cypher_info["result"]=result
            #end_time=time.time()
            #execute_time=end_time-start_time
            execute_time=summary.result_available_after+summary.result_consumed_after
            time1.append(execute_time)
            cypher_info["time"]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cyphers_info[line]=cypher_info
    with open(path+"/result/optwritetime.json",'w') as timefile:
        json.dump(cyphers_info,timefile,indent=4)
    return time1


def neo4j_test_coverafteropt(path,url,user,password):
    connector1=connector(url,user,password)
    print("successful connect")
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    output_path=path+"/output.json"
    my_tool=tool(output_path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    cyphers_info={}
    with open(path+"/recover/all.txt",'r') as inputfile:
        lines=inputfile.readlines()
        for line in lines:
            cypher_info={}
            line=line.rstrip("\n")
            print(line)
            new_line=my_tool.tran(line)
            result1,summary=connector1.run_cypher(new_line)
            result=str(result1)
            cypher_info["result"]=result
            #end_time=time.time()
            #execute_time=end_time-start_time
            execute_time=summary.result_available_after+summary.result_consumed_after
            cypher_info["time"]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cyphers_info[line]=cypher_info
    # with open(path+"/result/optcovertime.json",'w') as timefile:
    #     json.dump(cyphers_info,timefile,indent=4)

def test_write(path):
    eve_time={}
    all_records=[]
    ave_time=[]
    for it in range(5):
        time1=neo4j_test_writebeforeopt(path,neo4j_url1,neo4j_user1,neo4j_password1)
        all_records.append(time1)
        print("time1: ",time1)
        eve_time[str(it)]=time1
        neo4j_test_coverbeforeopt(path,neo4j_url1,neo4j_user1,neo4j_password1)
    all_time=[0]*len(all_records[0])
    for it in all_records:
        for i in range(len(it)):
            all_time[i]+=it[i]
    for itt in all_time:
        ave_time.append(itt/5.0)
    eve_time["all_time"]=all_time
    eve_time["ave_time"]=ave_time
    with open(path+"/result/all_writetime.json",'w') as timefile:
        json.dump(eve_time,timefile,indent=4)
def test_write_opt(path):
    eve_time={}
    all_records=[]
    ave_time=[]
    for it in range(5):
        time1=neo4j_test_writeafteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
        all_records.append(time1)
        print("time1: ",time1)
        eve_time[str(it)]=time1
        neo4j_test_coverafteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
    all_time=[0]*len(all_records[0])
    for it in all_records:
        for i in range(len(it)):
            all_time[i]+=it[i]
    for itt in all_time:
        ave_time.append(itt/5.0)
    eve_time["all_time"]=all_time
    eve_time["ave_time"]=ave_time
    with open(path+"/result/all_opt_writetime.json",'w') as timefile:
        json.dump(eve_time,timefile,indent=4)
def old_test():
    #neo4j_test_beforeopt(path,neo4j_url1,neo4j_user1,neo4j_password1)
    print("未优化读语句测试结束")
    test_write(path)
    print("未优化写语句测试结束")
def opt_test():
    #neo4j_test_afteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
    print("优化后读语句测试结束")
    test_write_opt(path)
    print("优化后写语句测试结束")
if __name__=="__main__":
    parse_args()
    old_thread=threading.Thread(target=old_test,name="OldThread")
    old_thread.start()
    opt_test()
    old_thread.join()
    print("测试结束")
    #old_test()
    #opt_test()