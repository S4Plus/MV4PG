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
tugraph_graph = 'finbenchSf10'
path="/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/finbench"
def filter(profile):
    result={}
    db=0
    dbHits=profile.get("dbHits")
    if dbHits is not None:
        db=dbHits
    rows=profile.get("rows")
    typee=profile.get("operatorType")
    args=profile.get("args")
    detail=args.get("Details")
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
    parser.add_argument('-tuurl', '--tuurl', help='url for tugraph')
    parser.add_argument('-tupwd', '--tupwd', help='pwd for tugraph')
    parser.add_argument('-tuuser', '--tuuser', help='user for tugraph')
    parser.add_argument('-neurl1', '--neurl1', help='url for neo4j1')
    parser.add_argument('-nepwd1', '--nepwd1', help='pwd for neo4j1')
    parser.add_argument('-neuser1', '--neuser1', help='user for neo4j1')
    parser.add_argument('-neurl2', '--neurl2', help='url for neo4j2')
    parser.add_argument('-nepwd2', '--nepwd2', help='pwd for neo4j2')
    parser.add_argument('-neuser2', '--neuser2', help='user for neo4j2')

    args = parser.parse_args()
    if args.path:
        global path
        path=args.path
    if args.tugraph:
        global tugraph_graph
        tugraph_graph=args.tugraph
    if args.tuurl:
        global tugraph_url
        tugraph_url=args.tuurl
    if args.tupwd:
        global tugraph_password
        tugraph_password=args.tupwd
    if args.tuuser:
        global tugraph_user
        tugraph_user=args.tuuser
    if args.neurl1:
        global neo4j_url1
        neo4j_url1=args.neurl1
    if args.nepwd1:
        global neo4j_password1
        neo4j_password1=args.nepwd1
    if args.neuser1:
        global neo4j_user1
        neo4j_user1=args.neuser1
    if args.neurl2:
        global neo4j_url2
        neo4j_url2=args.neurl2
    if args.nepwd2:
        global neo4j_password2
        neo4j_password2=args.nepwd2
    if args.neuser2:
        global neo4j_user2
        neo4j_user2=args.neuser2
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
                    self.dele_node.append(lists)
                elif index1 == 1:
                    self.add_rela+=lists
                elif index1 == 2:
                    self.dele_rela.append(lists)
    def tran(self,cypher:str):
        is_vetex=False
        result=[]
        delete_Start=cypher.find("delete")
        create_Start=cypher.find("create ")
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
                for it in self.dele_node:
                    new_cypher1=new_cypher+" UNWIND delete_views as view_vetex "
                    new_cypher2=" UNION "+new_cypher1
                    new_cypher1+=new_cypher2.join(it)
                    result.append(new_cypher1)
                result.append(cypher)
            else:
                for it in self.dele_rela:
                    new_cypher1=new_cypher+" UNWIND delete_views as view_edge "
                    new_cypher2=" UNION "+new_cypher1
                    new_cypher1+=new_cypher2.join(it)
                    result.append(new_cypher1)
                result.append(cypher)
            return result
        elif create_Start !=-1:
            edges=[]
            new=cypher[:create_Start]
            create_cypher=cypher[create_Start:]
            match_cypher="match "+cypher[create_Start+6:]
            match_cypher=new+match_cypher
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
                result.append(cypher)
                return result
            else:
                edge_cypher="["+",".join(edges)+"]"
                with_cypher=" WITH "+edge_cypher+" as "+"create_views "
                new_cypher=cypher+with_cypher
                new_cypher+=" UNWIND create_views as view_edge "
                new_cypher+=" with DISTINCT view_edge ".join(self.add_rela)
                result.append(new_cypher)
            return result
        result.append(cypher)
        return result
    

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
            new_line=line
            #new_line="profile "+new_line
            print(new_line)
            alltime=0.0
            for number in range(5):
                start_time=time.time()
                result1,summary=connector1.run_cypher(new_line)
                end_time=time.time()
                result=str(result1)
                cypher_info["result"+str(number)]=result
                execute_time=end_time-start_time
                #execute_time=summary.result_available_after+summary.result_consumed_after
                alltime+=execute_time
                # cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)
                cypher_info["time"+str(number)]=execute_time
                cypher_info["add_nodes"]=summary.counters.nodes_created
                cypher_info["delete_nodes"]=summary.counters.nodes_deleted
                cypher_info["add_rel"]=summary.counters.relationships_created
                cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cypher_info["alltime"]=alltime
            cypher_info["ave_time"]=alltime/5.0
            cyphers_info[line]=cypher_info
        with open(path+"/result/read/oldtime.json",'w') as timefile:
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
            start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            end_time=time.time()
            result=str(result1)
            cypher_info["result"]=result
            execute_time=end_time-start_time
            #execute_time=summary.result_available_after+summary.result_consumed_after
            time1.append(execute_time)
            cypher_info["time"]=execute_time
            #cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)          
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cyphers_info[line]=cypher_info
        with open(path+"/result/write/old_write_time.json",'w') as timefile:
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
            new_line=opt_cypher
            #new_line="profile "+new_line
            #print(new_line)
            alltime=0.0
            for number in range(5):
                start_time=time.time()
                result1,summary=connector1.run_cypher(new_line)
                end_time=time.time()
                result=str(result1)
                cypher_info["result"+str(number)]=result
                execute_time=end_time-start_time
                #execute_time=summary.result_available_after+summary.result_consumed_after
                alltime+=execute_time
                #cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)
                cypher_info["time"+str(number)]=execute_time
                cypher_info["add_nodes"]=summary.counters.nodes_created
                cypher_info["delete_nodes"]=summary.counters.nodes_deleted
                cypher_info["add_rel"]=summary.counters.relationships_created
                cypher_info["delete_rel"]=summary.counters.relationships_deleted
            cypher_info["alltime"]=alltime
            cypher_info["ave_time"]=alltime/5.0
            cyphers_info[new_line]=cypher_info
        with open(path+"/result/read/opttime.json",'w') as timefile:
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
            new_lines=my_tool.tran(line)
            #print(new_line)
            alltime=0
            result_end=[]
            for new_line in new_lines:
                cypher_info={}
                now_dict={}
                start_time=time.time()
                result1,summary=connector1.run_cypher(new_line)
                result=str(result1)
                now_dict["result"]=result
                end_time=time.time()
                execute_time=end_time-start_time
                alltime+=execute_time
                #execute_time=summary.result_available_after+summary.result_consumed_after
                now_dict["time"]=execute_time
                #now_dict["AllDbHits"],now_dict["profile"]=filter(summary.profile)          
                now_dict["add_nodes"]=summary.counters.nodes_created
                now_dict["delete_nodes"]=summary.counters.nodes_deleted
                now_dict["add_rel"]=summary.counters.relationships_created
                now_dict["delete_rel"]=summary.counters.relationships_deleted
                cypher_info[new_line]=now_dict
                result_end.append(cypher_info)
                result_end.append(alltime)            
            cyphers_info[line]=result_end
            time1.append(alltime)
    with open(path+"/result/write/opt_write_time.json",'w') as timefile:
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
    time1=[]
    cyphers_info={}
    with open(path+"/recover/all.txt",'r') as inputfile:
        lines=inputfile.readlines()    
        for line in lines:
            line=line.rstrip("\n")
            print(line)
            #print(line)
            new_lines=my_tool.tran(line)
            #print(new_line)
            result_end=[]
            for new_line in new_lines:
                cypher_info={}
                now_dict={}
                start_time=time.time()
                result1,summary=connector1.run_cypher(new_line)
                result=str(result1)
                now_dict["result"]=result
                end_time=time.time()
                execute_time=end_time-start_time
                #execute_time=summary.result_available_after+summary.result_consumed_after
                time1.append(execute_time)
                now_dict["time"]=execute_time
                #cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)          
                now_dict["add_nodes"]=summary.counters.nodes_created
                now_dict["delete_nodes"]=summary.counters.nodes_deleted
                now_dict["add_rel"]=summary.counters.relationships_created
                now_dict["delete_rel"]=summary.counters.relationships_deleted
                cypher_info[new_line]=now_dict
                result_end.append(cypher_info)            
            cyphers_info[line]=result_end
    with open(path+"/result/recovery/recovery.json",'w') as timefile:
        json.dump(cyphers_info,timefile,indent=4)
    return time1


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
    for index,it in enumerate(all_records):
        if index !=0:
            for i in range(len(it)):
                all_time[i]+=it[i]
    for itt in all_time:
        ave_time.append(itt/4.0)
    eve_time["all_time"]=all_time
    eve_time["ave_time"]=ave_time
    with open(path+"/result/write/all_writetime.json",'w') as timefile:
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
    for index,it in enumerate(all_records):
        if index !=0:
            for i in range(len(it)):
                all_time[i]+=it[i]
    for itt in all_time:
        ave_time.append(itt/4.0)
    eve_time["all_time"]=all_time
    eve_time["ave_time"]=ave_time
    with open(path+"/result/write/all_opt_writetime.json",'w') as timefile:
        json.dump(eve_time,timefile,indent=4)
def old_test():
    neo4j_test_beforeopt(path,neo4j_url1,neo4j_user1,neo4j_password1)
    print("未优化读语句测试结束")
    test_write(path)
    print("未优化写语句测试结束")
def opt_test():
    neo4j_test_afteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
    print("优化后读语句测试结束")
    test_write_opt(path)
    print("优化后写语句测试结束")
def mutex_test(path,url,user,password):
    mytool=tool(path+"/output.json")
    connector1=connector(url,user,password)
    f={}
    num=[1,10,100,1000,10000]
    for t in range(1):
        result={}
        alltimes=[]
        for n in num:
            alltime=0
            # alldbhints=0
            with open (path+"/WriteQueries/create_edge.cypher",'r') as createfile:
                lines=createfile.readlines()
                for line in lines:
                    new_lines=mytool.tran(line)
                    for i in range(n):
                        for new_line in new_lines:
                            connector1.run_cypher(new_line)
            with open (path+"/recover/recover_ce.cypher",'r') as delefile:
                lines=delefile.readlines()
                for line in lines:
                    new_lines=mytool.tran(line)
                    for new_line in new_lines:
                        time1=[]
                        starttime=time.time()
                        result1,summary=connector1.run_cypher(new_line)
                        endtime=time.time()
                        # dbhints,a=filter(summary.profile) 
                        excute_time=endtime-starttime
                        alltime+=excute_time
                        # alldbhints+=dbhints
                        time1.append(excute_time)
                        result[new_line+str(n)]=time1
                    result["alltime"+str(n)]=alltime
                    # result["alldbhints"+str(n)]=alldbhints
                    alltimes.append(alltime)
        with open (path+"/result/opt_db.json",'w') as outfile:
            json.dump(result,outfile,indent=4)
        print(alltimes)
        f[str(t)]=alltimes
    with open (path+"/result/opt10000.json",'w') as iss:
        json.dump(f,iss,indent=4)
def mutex_test2(path,url,user,password):
    connector1=connector(url,user,password)
    num=[1,10,100,1000,10000]
    f={}
    for t in range(1):
        result={}
        alltimes=[]
        for n in num:
            with open (path+"/WriteQueries/create_edge.cypher",'r') as createfile:
                lines=createfile.readlines()
                for line in lines:
                    new_line=line
                    for i in range(n):
                        starttime=time.time()
                        connector1.run_cypher(new_line)
                        endtime=time.time()
                        excute_time=endtime-starttime
            with open (path+"/recover/recover_ce.cypher",'r') as delefile:
                lines=delefile.readlines()
                for line in lines:
                    #time1=[]
                    f={}
                    new_line=line
                    starttime=time.time()
                    result1,summary=connector1.run_cypher(new_line)
                    endtime=time.time()
                    # dbhints,a=filter(summary.profile) 
                    excute_time=endtime-starttime
                    f["time"]=excute_time
                    # f["alldbhints"]=dbhints
                    result[line+str(n)]=f
                    alltimes.append(excute_time)
        
        with open (path+"/result/old_db.json",'w') as outfile:
            json.dump(result,outfile,indent=4)
        print(alltimes)
        f[str(t)]=alltimes
    with open (path+"/result/old10000.json",'w') as iss:
        json.dump(f,iss,indent=4)
if __name__=="__main__":
    parse_args()
    mutex_test(path,neo4j_url2,neo4j_user2,neo4j_password2)
    mutex_test2(path,neo4j_url1,neo4j_user1,neo4j_password1)
    old_thread=threading.Thread(target=old_test,name="OldThread")
    old_thread.start()
    opt_test()
    old_thread.join()
    print("测试结束")