#!/bin/python

import argparse
from TuGraphClient import TuGraphClient
import time
import os

ip = '127.0.0.1'
old_port = '7071'
port = '7072'
# old_graph = 'ldbcSf1'
graph = 'ldbcSf1'
user = 'admin'
password = '73@TuGraph'
cypher = 'match (n:Comment)-[r:replyOf*..]->(m:Post) return count(m)'
cypher_folder="/tugraph-db_graph_views/view_test/test_queries"
cycle = 5
output_path="/tugraph-db_graph_views/view_test/optimization.txt"

def parse_args():
    parser = argparse.ArgumentParser(description="TuGraph Rpc Client for python")
    parser.add_argument('-i', '--ip', help='ip for graph server')
    parser.add_argument('-p', '--port', help='port for graph server')
    parser.add_argument('-g', '--graph', help='graph name')
    parser.add_argument('-u', '--user', help='user name')
    parser.add_argument('-c', '--cypher', help='cypher to query')
    parser.add_argument('--password', help='user password')
    parser.add_argument('--cycle', help='cycle')
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
    if args.cycle:
        global cycle
        cycle = int(args.cycle)


def call_cypher(input_port,input_graph):
    url = ip + ":" + input_port
    client = TuGraphClient(url, user, password,input_graph)
    try:
        ret = client.call_cypher(cypher)['result']
        return True,ret
    except Exception as e:
        return False,""

def test_cypher(cypher):
    ave_time=0
    optimized_ave_time=0
    for i in range(0,cycle):
        start=time.time()
        try:
            is_success,records=call_cypher(old_port,graph)
        except Exception as e:
            print("error")
        end=time.time()
        print("cypher duration:",end-start)
        ave_time+=end-start
    for i in range(0,cycle):
        start=time.time()
        try:
            is_success,optimized_records=call_cypher(port,graph)
        except Exception as e:
            print("error")
        end=time.time()
        print("optimized cypher duration:",end-start)
        optimized_ave_time+=end-start
    ave_time/=cycle
    optimized_ave_time/=cycle

    f=open(output_path,"a")
    f.write(cypher+"\n")
    f.write("cycle: "+str(cycle)+"\n")
    f.write("original_time: "+str(ave_time)+"\n")
    f.write("optimized_time: "+str(optimized_ave_time)+"\n")
    f.write("speed up: "+str(ave_time/optimized_ave_time)+"\n")
    if(len(records)<11):
        for i in range(0,len(records)):
            if(records[i]!=optimized_records[i]):
                print("wrong: "+cypher)
                print("records:"+str(records[i]))
                print("optimized_records:"+str(optimized_records[i]))
            f.write(str(records[i])+"\n")
            f.write(str(optimized_records[i])+"\n")
    f.write("\n")

if  __name__ == '__main__':
    parse_args()

    for file in os.listdir(cypher_folder):
        with open(os.path.join(cypher_folder,file)) as f:
            cypher=f.read()
            test_cypher(cypher)

    # test_cypher(cypher)
    