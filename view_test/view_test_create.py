#!/bin/python

import argparse
from TuGraphClient import TuGraphClient
import time
import re
import sys


ip = '127.0.0.1'
port = '7072'
graph = 'ldbcSf1'
user = 'admin'
password = '73@TuGraph'

is_delete=False

delete_cypher="CALL db.deleteLabel('edge', 'ROOT_POST')"
cypher='create view ROOT_POST as ( Construct (n)-[r:ROOT_POST]->(m) match (n:Comment)-[:replyOf*..]->(m:Post) )'
# cypher='match (n:Comment)-[r:replyOf*..]->(m:Post) return count(m)'
# cypher ='match (n:Comment{id:561})-[r:replyOf]->(m) with r limit 1 return r'
# cypher ='match (n:Comment{id:561}),(m:Post{id:556}) with n,m create (n)-[:replyOf{creationDate:1266568959307}]->(m)'
# cypher = 'create view test as match (n:person)-[]->()-[]->(m:keyword) return n,m'
# cypher='create view two_hop_friend as match (n:user)-[:is_friend*..2]->(m:user) return n,m'
# cypher = 'MATCH (a)-[]->(n:user)-[:is_friend*..2]->(m:user) return m'
# cypher = 'MATCH (n:person)-[r]->()-[]->(m:keyword) return m'
# cypher = 'MATCH (m:keyword)<-[r]-()<-[]-(n:person) return m'
# cypher = 'MATCH (m:keyword)<-[]-()<-[]-(n:person) return count(n)'
output_path="/tugraph-db_graph_views/view_test/create_log.txt"

def parse_args():
    parser = argparse.ArgumentParser(description="TuGraph Rpc Client for python")
    parser.add_argument('-i', '--ip', help='ip for graph server')
    parser.add_argument('-p', '--port', help='port for graph server')
    parser.add_argument('-g', '--graph', help='graph name')
    parser.add_argument('-u', '--user', help='user name')
    parser.add_argument('-c', '--cypher', help='cypher to query')
    parser.add_argument('--password', help='user password')
    parser.add_argument('--is_delete', help='is delete')
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
    if args.is_delete:
        global is_delete
        if(args.is_delete=="True" or args.is_delete=="true"):
            is_delete=True
        else:
            is_delete=False



def call_cypher(cypher):
    url = ip + ":" + port
    print(cypher)
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
        if(is_delete):
            call_cypher(delete_cypher)
        start_time=time.time()
        is_success,records=call_cypher(cypher)
        end_time=time.time()
        # print("查询时间：",end_time-start_time)
        f=open(output_path,'a')
        f.write("create view cypher: "+cypher+"\n")
        f.write("create time: "+str(end_time-start_time)+"\n")
        print(records[0])
        # f=open("edge.csv",'w')
        # min_id=100000000
        # max_id=0
        # for record in records:
        #     if(record[0]<min_id):
        #         min_id=record[0]
        #     if(record[0]>max_id):
        #         max_id=record[0]
        #     if(record[1]<min_id):
        #         min_id=record[1]
        #     if(record[1]>max_id):
        #         max_id=record[1]
        # for record in records:
        #     f.write(str(record[0]-min_id)+","+str(record[1]-min_id)+",knows\n")
        # f2=open("node.csv",'w')
        # for i in range(min_id,max_id+1):
        #     f2.write(str(i-min_id)+"\n")
        #     # print(record[0])
    except Exception as e:
        print(e)
    