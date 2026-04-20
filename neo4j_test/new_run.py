from neo4j import GraphDatabase
import json
import logging
from TuGraphClient import TuGraphClient
import time
import argparse
import os
import threading
logging.basicConfig(level=logging.ERROR)
neo4j_url1="bolt://localhost:7687" #没有试图
neo4j_user1="neo4j"
neo4j_password1="123456"
neo4j_url2="bolt://localhost:7688" 
neo4j_user2="neo4j" 
neo4j_password2="352541141"
tugraph_url = '127.0.0.1:7073'
tugraph_user = 'admin'
tugraph_password = '73@TuGraph'
tugraph_graph = 'ldbcSf10'
path="./ldbcSf10_new"
is_profile=False
cycle=5
test_type=0

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
            # print("query:",query)
            # print("result size:",result.peek())
            records = [record for record in result]
            # 获取查询的 summary
            summary = result.consume()
            # print("summary:",summary)
            return records, summary

def str2bool(v):
    if isinstance(v, bool):
        return v
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')
    
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
    parser.add_argument('-pr', '--profile', default=False, help='whether to use profile')
    parser.add_argument('-c', '--cycle', type=int, default=5, help='number of cycles to run')
    parser.add_argument('-tt', '--test_type', type=int, default=0, help='test type: 1=Read,2=Write,3=multi_delete')

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
    if args.profile:
        global is_profile
        is_profile=str2bool(args.profile)
    if args.cycle:
        global cycle
        cycle=args.cycle
    if args.test_type is not None:
        global test_type
        test_type=args.test_type

def get_lines(path, kind: str):
    # kind: 'read' or 'write' or 'recover' etc.
    file_map = {
        'read': path + '/ReadQueries/all.txt',
        'write': path + '/WriteQueries/all.txt',
        'recover': path + '/recover/all.txt'
    }
    fp = file_map.get(kind)
    if not fp or not os.path.exists(fp):
        return []
    with open(fp,'r') as f:
        lines = [l.rstrip('\n') for l in f.readlines()]
    if test_type == 3:
        return [l for l in lines if '#sym:mutex_test' in l]
    return lines
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
            # print(is_vetex)
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
            # new=cypher[:create_Start]
            create_cypher=cypher[create_Start:]
            # match_cypher=create_cypher.replace("CREATE"," MATCH ")
            # match_cypher=new+match_cypher
            #print(create_cypher)
            for index,it in enumerate(create_cypher):
                if it=="[":
                    i=index
                    while(i<len(create_cypher)):
                        if create_cypher[i]==":" or create_cypher[i]=="]":
                            break
                        i+=1
                    if create_cypher[i]==":":
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
    lines = get_lines(path, 'read')
    # lines already stripped of newlines by get_lines
    cyphers_info={}
    for line in lines:
        #print(line)
        cypher_info={}
        new_line=line
        if is_profile:
            new_line="profile "+new_line
        print(new_line)
        alltime=0.0
        for number in range(cycle):
            start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            end_time=time.time()
            result=str(result1)
            cypher_info["result"+str(number)]=result
            if ~is_profile:
                execute_time=end_time-start_time
            else:
                execute_time=summary.result_available_after+summary.result_consumed_after
            alltime+=execute_time
            if is_profile:
                cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)
            cypher_info["time"+str(number)]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
        cypher_info["alltime"]=alltime
        cypher_info["ave_time"]=alltime/float(cycle)
        cyphers_info[line]=cypher_info
        os.makedirs(path+"/result/read",exist_ok=True)
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
    lines = get_lines(path, 'write')
    # lines already stripped of newlines by get_lines
    cyphers_info={}
    for line in lines:
        #print(line)
        cypher_info={}
        new_line=line
        print(new_line)
        start_time=time.time()
        result1,summary=connector1.run_cypher(new_line)
        print(f"server result_consumed_after: {summary.result_consumed_after} ms")
        end_time=time.time()
        result=str(result1)
        cypher_info["result"]=result
        if ~is_profile:
            execute_time=end_time-start_time
        else:
            execute_time=summary.result_available_after+summary.result_consumed_after
        time1.append(execute_time)
        cypher_info["time"]=execute_time
        if is_profile:
            cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)          
        cypher_info["add_nodes"]=summary.counters.nodes_created
        cypher_info["delete_nodes"]=summary.counters.nodes_deleted
        cypher_info["add_rel"]=summary.counters.relationships_created
        cypher_info["delete_rel"]=summary.counters.relationships_deleted
        cyphers_info[line]=cypher_info
        os.makedirs(path+"/result/write",exist_ok=True)
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
    lines = get_lines(path, 'recover')
    for line in lines:
        # lines already stripped
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
    lines = get_lines(path, 'read')
    cyphers_info={}
    for line in lines:
        # lines already stripped
        line="Optimize "+line
        print(line)
        opt_start_time=time.time()
        opt_cypher_list = client.call_cypher(line)['result']
        opt_end_time=time.time()
        opt_cypher=listtostr(opt_cypher_list)
        print("opt_cypher:",opt_cypher)
        cypher_info={}
        opt_time=opt_end_time-opt_start_time
        cypher_info["opt_time"]=opt_time
        new_line=opt_cypher
        if is_profile:
            new_line="profile "+new_line
        #print(new_line)
        alltime=0.0
        for number in range(cycle):
            start_time=time.time()
            result1,summary=connector1.run_cypher(new_line)
            end_time=time.time()
            result=str(result1)
            cypher_info["result"+str(number)]=result
            if ~is_profile:
                execute_time=end_time-start_time
            else:
                execute_time=summary.result_available_after+summary.result_consumed_after
            alltime+=execute_time
            if is_profile:
                cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)
            cypher_info["time"+str(number)]=execute_time
            cypher_info["add_nodes"]=summary.counters.nodes_created
            cypher_info["delete_nodes"]=summary.counters.nodes_deleted
            cypher_info["add_rel"]=summary.counters.relationships_created
            cypher_info["delete_rel"]=summary.counters.relationships_deleted
        cypher_info["alltime"]=alltime
        cypher_info["ave_time"]=alltime/float(cycle)
        cyphers_info[new_line]=cypher_info
        os.makedirs(path+"/result/read",exist_ok=True)
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
    lines = get_lines(path, 'write')
    for line in lines:
        # lines already stripped
        print("write:",line)
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
            end_time=time.time()
            result=str(result1)
            now_dict["result"]=result
            if ~is_profile:
                execute_time=end_time-start_time
            else:
                execute_time=summary.result_available_after+summary.result_consumed_after
            alltime+=execute_time
            now_dict["time"]=execute_time
            if is_profile:
                now_dict["AllDbHits"],now_dict["profile"]=filter(summary.profile)          
            now_dict["add_nodes"]=summary.counters.nodes_created
            now_dict["delete_nodes"]=summary.counters.nodes_deleted
            now_dict["add_rel"]=summary.counters.relationships_created
            now_dict["delete_rel"]=summary.counters.relationships_deleted
            cypher_info[new_line]=now_dict
            result_end.append(cypher_info)
            result_end.append(alltime)            
        cyphers_info[line]=result_end
        time1.append(alltime)
    os.makedirs(path+"/result/write",exist_ok=True)
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
    lines = get_lines(path, 'recover')
    for line in lines:
        # lines already stripped
        print("cover:",line)
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
            if ~is_profile:
                execute_time=end_time-start_time
            else:
                execute_time=summary.result_available_after+summary.result_consumed_after
            time1.append(execute_time)
            now_dict["time"]=execute_time
            if is_profile:
                cypher_info["AllDbHits"],cypher_info["profile"]=filter(summary.profile)          
            now_dict["add_nodes"]=summary.counters.nodes_created
            now_dict["delete_nodes"]=summary.counters.nodes_deleted
            now_dict["add_rel"]=summary.counters.relationships_created
            now_dict["delete_rel"]=summary.counters.relationships_deleted
            cypher_info[new_line]=now_dict
            result_end.append(cypher_info)            
        cyphers_info[line]=result_end

        os.makedirs(path+"/result/recovery",exist_ok=True)
        with open(path+"/result/recovery/recovery.json",'w') as timefile:
            json.dump(cyphers_info,timefile,indent=4)
    return time1


def test_write(path):
    eve_time={}
    all_records=[]
    ave_time=[]
    for it in range(cycle):
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
        if cycle > 1:
            ave_time.append(itt/float(cycle-1))
    eve_time["all_time"]=all_time
    eve_time["ave_time"]=ave_time
    with open(path+"/result/write/all_writetime.json",'w') as timefile:
        json.dump(eve_time,timefile,indent=4)
def test_write_opt(path):
    eve_time={}
    all_records=[]
    ave_time=[]
    for it in range(cycle):
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
        if cycle > 1:
            ave_time.append(itt/float(cycle-1))
    eve_time["all_time"]=all_time
    eve_time["ave_time"]=ave_time
    os.makedirs(path+"/result/write",exist_ok=True)
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

def mutex_test(path,url,user,password,is_opt):
    mytool=tool(path+"/output.json")
    connector1=connector(url,user,password)
    f={}
    num=[1,10,100,1000,10000]
    # num=[1,10,100]
    output_detail_path=path+"/result/old_db.json"
    if is_opt:
        output_detail_path=path+"/result/opt_db.json"
    output_path=path+"/result/old10000.json"
    if is_opt:
        output_path=path+"/result/opt10000.json"
    result={}
    for n in num:
        alltimes=[]
        result["alltime"+str(n)]=0
        for t in range(cycle):
            result["alltime"+str(n)+"_"+str(t)]=0
            alltime=0
            if is_profile:
                alldbhints=0
            with open (path+"/MultiDeleteTest/create_edge.cypher",'r') as createfile:
                lines=createfile.readlines()
                for line in lines:
                    new_lines=[line]
                    if is_opt:
                        new_lines=mytool.tran(line)
                    for i in range(n):
                        for new_line in new_lines:
                            connector1.run_cypher(new_line)
            with open (path+"/MultiDeleteTest/recover_ce.cypher",'r') as delefile:
                lines=delefile.readlines()
                for line in lines:
                    new_lines=[line]
                    if is_opt:
                        new_lines=mytool.tran(line)
                    for new_line in new_lines:
                        print("new_line:",new_line)
                        time1=[]
                        if is_profile:
                            new_line="profile "+new_line
                        starttime=time.time()
                        result1,summary=connector1.run_cypher(new_line)
                        endtime=time.time()
                        if is_profile:
                            dbhints,a=filter(summary.profile) 
                        excute_time=endtime-starttime
                        alltime+=excute_time
                        if is_profile:
                            alldbhints+=dbhints
                        time1.append(excute_time)
                        if new_line+str(n) in result:
                            result[new_line+str(n)]+=excute_time
                        else:
                            result[new_line+str(n)]=excute_time
                    result["alltime"+str(n)]+=alltime
                    result["alltime"+str(n)+"_"+str(t)]+=alltime
                    if is_profile:
                        result["alldbhints"+str(n)]=alldbhints
            alltimes.append({t: result["alltime"+str(n)+"_"+str(t)]})
        with open (output_detail_path,'w') as outfile:
            json.dump(result,outfile,indent=4)
            print(alltimes)
            alltimes.append({'avg': result["alltime"+str(n)]/cycle})
            f[n]=alltimes
        print("n=",n," alltime:",result["alltime"+str(n)])
    with open (output_path,'w') as iss:
        json.dump(f,iss,indent=4)

# def mutex_test2(path,url,user,password):
#     connector1=connector(url,user,password)
#     num=[1,10,100,1000,10000]
#     # num=[1]
#     f={}
#     result={}
#     alltimes=[]
#     for n in num:
#         result["alltime"+str(n)]=0
#         for t in range(cycle):
#             result["alltime"+str(n)+"_"+str(t)]=0
#             with open (path+"/MultiDeleteTest/create_edge.cypher",'r') as createfile:
#                 lines=createfile.readlines()
#                 for line in lines:
#                     new_line=line
#                     for i in range(n):
#                         starttime=time.time()
#                         connector1.run_cypher(new_line)
#                         endtime=time.time()
#                         excute_time=endtime-starttime
#             with open (path+"/MultiDeleteTest/recover_ce.cypher",'r') as delefile:
#                 lines=delefile.readlines()
#                 for line in lines:
#                     #time1=[]
#                     f1={}
#                     new_line=line
#                     starttime=time.time()
#                     result1,summary=connector1.run_cypher(new_line)
#                     endtime=time.time()
#                     # dbhints,a=filter(summary.profile) 
#                     excute_time=endtime-starttime
#                     f1["time"]=excute_time
#                     # f["alldbhints"]=dbhints
#                     # result[line+str(n)]=f
#                     if line+str(n) in result:
#                         result[line+str(n)]+=excute_time
#                     else:
#                         result[line+str(n)]=excute_time
#                     result["alltime"+str(n)]+=excute_time
#                     result["alltime"+str(n)+"_"+str(t)]+=excute_time
#         alltimes.append(result["alltime"+str(n)]/cycle)
        
#         with open (path+"/result/old_db.json",'w') as outfile:
#             json.dump(result,outfile,indent=4)
#         print(alltimes)
#         f[n]=alltimes
#     with open (path+"/result/old10000.json",'w') as iss:
#         json.dump(f,iss,indent=4)

if __name__=="__main__":
    parse_args()

    print("测试开始")
    if test_type == 1:
        # Read only
        neo4j_test_beforeopt(path,neo4j_url1,neo4j_user1,neo4j_password1)
        print("未优化读语句测试结束")
        neo4j_test_afteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
        print("优化后读语句测试结束")
    elif test_type == 2:
        # Write only
        test_write(path)
        print("未优化写语句测试结束")
        test_write_opt(path)
        print("优化后写语句测试结束")
    elif test_type == 3:
        mutex_test(path,neo4j_url2,neo4j_user2,neo4j_password2,True)
        mutex_test(path,neo4j_url1,neo4j_user1,neo4j_password1,False)
    else:
        # default or multi_delete (test_type==3 will be filtered by get_lines)
        old_thread=threading.Thread(target=old_test,name="OldThread")
        old_thread.start()
        opt_test()
        old_thread.join()
    print("测试结束")

    # neo4j_test_writeafteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
    # neo4j_test_coverafteropt(path,neo4j_url2,neo4j_user2,neo4j_password2)
    # test_write_opt(path)
