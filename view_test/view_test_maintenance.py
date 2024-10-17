#!/bin/python

import argparse
from TuGraphClient import TuGraphClient
import time
import re


ip = '127.0.0.1'
port = '7072'
# graph = 'MovieDemo1'
graph='ldbcSf1'
user = 'admin'
password = '73@TuGraph'
count_cypher='match (n)-[r:ROOT_POST]->(m) return count(r)'
# create_cypher = 'match (n:person{id:5}),(m:movie{id:28}) create (n)-[r:write]->(m)'
# delete_cypher= 'match (n:person{id:5})-[r NoDupEdge]->(m:movie{id:28}) delete r'
# create_cypher='match (n:user{id:3}),(m:user{id:142}) create (n)-[r:is_friend]->(m)'
# delete_cypher='match (n:user{id:3})-[r NoDupEdge]->(m:user{id:142}) delete r'
create_first=False
delete_cypher ='match (n:Comment{id:558})-[r:replyOf]->(m:Post{id:556}) with r limit 1 delete r'
# 维护语句：match (n:Comment)-[:replyOf*0..]->(:Comment{id:558})-[ANONR1:replyOf]->(:Post{id:556})-[:replyOf*0..]->(m:Post) where id(ANONR1)=0  WITH n,m CREATE (n)-[r:ROOT_POST]->(m)
create_cypher ='match (n:Comment{id:558}),(m:Post{id:556}) with n,m create (n)-[:replyOf{creationDate:1266604713724}]->(m)'
# create_cypher='match (n:Comment{id:557}),(m:Post{id:556}) create (n)-[r:replyOf{creationDate:20240517}]->(m)'
# delete_cypher='match (n:Comment{id:557})-[r:replyOf nodupedge]->(m:Post{id:556}) delete r'
create_time=5
output_path="/tugraph-db_graph_views/view_test/maintenance_log.txt"

def parse_args():
    parser = argparse.ArgumentParser(description="TuGraph Rpc Client for python")
    parser.add_argument('-i', '--ip', help='ip for graph server')
    parser.add_argument('-p', '--port', help='port for graph server')
    parser.add_argument('-g', '--graph', help='graph name')
    parser.add_argument('-u', '--user', help='user name')
    parser.add_argument('-c', '--cypher', help='cypher to query')
    parser.add_argument('--password', help='user password')
    args = parser.parse_args()
    if args.ip:
        global ip
        ip = args.ip
    if args.port:
        global port
        port = args.port
    if args.graph:
        global graph
        graph = args.graph
    if args.user:
        global user
        user = args.user
    if args.password:
        global password
        password = args.password
    if args.cypher:
        global cypher
        cypher = args.cypher


def call_cypher(cypher):
    url = ip + ":" + port
    # print(cypher)
    client = TuGraphClient(url, user, password,graph)
    
    try:
        ret = client.call_cypher(cypher)['result']
        return True,ret
    except Exception as e:
        return False,""

if  __name__ == '__main__':
    parse_args()
    # cypher=input("请输入视图创建语句：")

    # cypher = re.sub(r"\s+", " ", cypher).lower()
    # if(cypher[0]==' '):
    #     cypher=cypher[1:]
    # print(cypher)
    # if(not cypher.startswith("create view")):
    #     print("不是视图创建语句")
    #     exit()
    # view_name=cypher.split(" ")[2]

    try:
        is_success,records=call_cypher(count_cypher)
        initial_count=records[0][0]
        now_count=initial_count
        print("初始边数：",initial_count)
        delta_count=0
        delete_duration=0
        create_duration=0
        if(create_first):
            for i in range(create_time):
                start_time=time.time()
                call_cypher(create_cypher)
                end_time=time.time()

                is_success,records=call_cypher(count_cypher)
                create_duration+=(end_time-start_time)
                if(delta_count>0 and abs(records[0][0]-now_count)!=delta_count):
                    print("每次创建的边数不一致")
                delta_count=abs(records[0][0]-now_count)
                now_count=records[0][0]
                print("创建第"+str(i+1)+"次后的边数:"+str(now_count))
            for i in range(create_time):
                start_time=time.time()
                call_cypher(delete_cypher)
                end_time=time.time()

                is_success,records=call_cypher(count_cypher)
                delete_duration+=(end_time-start_time)
                if(delta_count>0 and abs(records[0][0]-now_count)!=delta_count):
                    print("每次删除的边数不一致")
                delta_count=abs(records[0][0]-now_count)
                now_count=records[0][0]
                print("删除第"+str(i+1)+"次后的边数:"+str(now_count))
        else:
            
            for i in range(create_time):
                start_time=time.time()
                call_cypher(delete_cypher)
                end_time=time.time()
                is_success,records=call_cypher(count_cypher)
                delete_duration+=(end_time-start_time)
                if(delta_count>0 and abs(records[0][0]-now_count)!=delta_count):
                    print("每次删除的边数不一致:"+str(delta_count))
                delta_count=abs(records[0][0]-now_count)
                now_count=records[0][0]
                print("删除第"+str(i+1)+"次后的边数:"+str(now_count))

                start_time=time.time()
                call_cypher(create_cypher)
                end_time=time.time()
                is_success,records=call_cypher(count_cypher)
                create_duration+=(end_time-start_time)
                if(delta_count>0 and abs(records[0][0]-now_count)!=delta_count):
                    print("每次创建的边数不一致:"+str(delta_count))
                delta_count=abs(records[0][0]-now_count)
                now_count=records[0][0]
                print("创建第"+str(i+1)+"次后的边数:"+str(now_count))
            # for i in range(create_time):
            #     call_cypher(create_cypher)
            #     is_success,records=call_cypher(count_cypher)
            #     if(delta_count>0 and abs(records[0][0]-now_count)!=delta_count):
            #         print("每次创建的边数不一致")
            #     delta_count=abs(records[0][0]-now_count)
            #     now_count=records[0][0]
            #     print("创建第"+str(i+1)+"次后的边数:"+str(now_count))
        if(now_count==initial_count):
            f=open(output_path,'a')
            if(create_first):
                f.write("create cypher: "+create_cypher+'\n')
                f.write("       ave time: "+str(create_duration/create_time)+'\n')
                f.write("delete cypher: "+delete_cypher+'\n')
                f.write("       ave time: "+str(delete_duration/create_time)+'\n')
            else:
                f.write("delete cypher: "+delete_cypher+'\n')
                f.write("       ave time: "+str(delete_duration/create_time)+'\n')
                f.write("create cypher: "+create_cypher+'\n')
                f.write("       ave time: "+str(create_duration/create_time)+'\n')
            f.write("total time: "+str(create_duration+delete_duration)+'\n')
            f.write("cycle: "+str(create_time)+'\n')
            f.write("total ave time: "+str((create_duration+delete_duration)/(2*create_time))+'\n')
            f.write("\n")
            print("测试通过")

    except Exception as e:
        print("error")
    