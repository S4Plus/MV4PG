#!/bin/python

import argparse
from TuGraphClient import TuGraphClient
import time
import re
import sys
import os


ip = '127.0.0.1'
old_port = '7071'
port = '7072'
graph = 'ldbcSf1'
folder_name = ''
user = 'admin'
password = '73@TuGraph'

root_folder="../view_test"
# view_folder="/tugraph-db_graph_views/view_test/views"
# output_path=""

def parse_args():
    parser = argparse.ArgumentParser(description="TuGraph Rpc Client for python")
    parser.add_argument('-i', '--ip', help='ip for graph server')
    parser.add_argument('-p', '--port', help='port for graph server')
    parser.add_argument('-o', '--old_port', help='original port for graph server')
    parser.add_argument('-g', '--graph', help='graph name')
    parser.add_argument('-u', '--user', help='user name')
    parser.add_argument('-c', '--cypher', help='cypher to query')
    parser.add_argument('-f', '--folder', help='folder name')
    parser.add_argument('--password', help='user password')
    parser.add_argument('--is_delete', help='is delete')
    args = parser.parse_args()
    if args.ip:
        global ip
        ip = args.ip
    if args.port:
        global port
        port = args.port
    if args.old_port:
        global old_port
        old_port = args.old_port
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
    if args.folder:
        global folder_name
        folder_name = args.folder
    if args.is_delete:
        global is_delete
        if(args.is_delete=="True" or args.is_delete=="true"):
            is_delete=True
        else:
            is_delete=False



def call_cypher(input_cypher,port,graph):
    url = ip + ":" + port
    print("call cypher:",input_cypher)
    client = TuGraphClient(url, user, password,graph)
    
    try:
        print("create start")
        ret = client.call_cypher(input_cypher)['result']
        print("create end")
        return True,ret
    except Exception as e:
        return False,""

def test_cypher(input_cypher):
    ave_time=0
    optimized_ave_time=0

    start=time.time()
    try:
        is_success,records=call_cypher(input_cypher,old_port,graph)
    except Exception as e:
        print("error")
    end=time.time()
    print("cypher duration:",end-start)
    ave_time+=end-start

    start=time.time()
    try:
        is_success,optimized_records=call_cypher(input_cypher, port,graph)
    except Exception as e:
        print("error")
    end=time.time()
    print("optimized cypher duration:",end-start)
    optimized_ave_time+=end-start

    f=open(output_path,"a")
    f.write(input_cypher+"\n")
    f.write("original_time: "+str(ave_time)+"\n")
    f.write("optimized_time: "+str(optimized_ave_time)+"\n")
    f.write("speed up: "+str(ave_time/optimized_ave_time)+"\n")
    if(len(records)<11):
        for i in range(0,len(records)):
            if(records[i]!=optimized_records[i]):
                print("wrong: "+input_cypher)
                print("records:"+str(records[i]))
                print("optimized_records:"+str(optimized_records[i]))
            f.write(str(records[i])+"\n")
            f.write(str(optimized_records[i])+"\n")
    f.write("\n")

if  __name__ == '__main__':
    parse_args()
    global output_path
    # cypher=input("请输入视图创建语句：")

    # cypher = re.sub(r"\s+", " ", cypher).lower()
    # if(cypher[0]==' '):
    #     cypher=cypher[1:]
    # print(cypher)
    # if(not cypher.startswith("create view")):
    #     print("不是视图创建语句")
    #     exit()
    # view_name=cypher.split(" ")[2]
    if folder_name=='':
        folder_name=graph
    view_folder=os.path.join(root_folder,folder_name,"recover")
    output_path=os.path.join(root_folder,folder_name,"recover.txt")
    for file in os.listdir(view_folder):       
        lines = open(view_folder+"/"+file).readlines()
        for line in lines:
            print(line)
            test_cypher(line)
        #     start_time=time.time()
        #     cypher = line
        #     is_success,records=call_cypher(cypher)
        #     end_time=time.time()
        # # print("查询时间：",end_time-start_time)
        #     f=open(output_path,'a')
        #     f.write("recover cypher: "+cypher+"\n")
        #     f.write("create time: "+str(end_time-start_time)+"\n\n")
        #     print(records[0])
        #     f.close()
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
    # except Exception as e:
    #     print(e)
    