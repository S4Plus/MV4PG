#!/bin/python

import argparse
from TuGraphClient import TuGraphClient
import time
import os
import json

ip = '127.0.0.1'
old_port = '7071'
port = '7072'
# old_graph = 'ldbcSf1'
graph = 'ldbcSf1'
folder_name = ''
user = 'admin'
password = '73@TuGraph'
# cypher = 'match (n:Comment)-[r:replyOf*..]->(m:Post) return count(m)'
root_folder="../view_test/"
# parameter_folder="/tugraph-db_graph_views/view_test/finbench_parameter"
cycle = 5
# output_path=""
is_profile=False

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
    parser = argparse.ArgumentParser(description="TuGraph Rpc Client for python")
    parser.add_argument('-i', '--ip', help='ip for graph server')
    parser.add_argument('-p', '--port', help='port for graph server')
    parser.add_argument('-o', '--old_port', help='original port for graph server')
    parser.add_argument('-g', '--graph', help='graph name')
    parser.add_argument('-u', '--user', help='user name')
    parser.add_argument('-c', '--cypher', help='cypher to query')
    parser.add_argument('-f', '--folder', help='folder name')
    parser.add_argument('-pr', '--profile', help='is profile')
    parser.add_argument('--password', help='user password')
    parser.add_argument('--cycle', help='cycle')
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
    if args.profile:
        global is_profile
        is_profile=str2bool(args.profile)
    if args.cycle:
        global cycle
        cycle = int(args.cycle)


def call_cypher(input_cypher, input_port,input_graph):
    url = ip + ":" + input_port
    client = TuGraphClient(url, user, password,input_graph)
    try:
        ret = client.call_cypher(input_cypher)['result']
        return True,ret
    except Exception as e:
        return False,""

def test_cypher(input_cypher):
    if(is_profile):
        input_cypher="PROFILE "+input_cypher
    original_time=0
    optimized_time=0

    start=time.time()
    try:
        is_success,records=call_cypher(input_cypher,old_port,graph)
    except Exception as e:
        print("error")
    end=time.time()
    print("cypher duration:",end-start)
    original_time=end-start

    start=time.time()
    try:
        is_success,optimized_records=call_cypher(input_cypher, port,graph)
    except Exception as e:
        print("error")
    end=time.time()
    print("optimized cypher duration:",end-start)
    optimized_time=end-start

    return original_time,optimized_time
    # f=open(output_path,"a")
    # f.write(input_cypher+"\n")
    # f.write("cycle: "+str(cycle)+"\n")
    # f.write("original_time: "+str(original_time)+"\n")
    # f.write("optimized_time: "+str(optimized_time)+"\n")
    # f.write("speed up: "+str(original_time/optimized_time)+"\n")
    # if(len(records)<11):
    #     for i in range(0,len(records)):
    #         if(records[i]!=optimized_records[i]):
    #             print("wrong: "+input_cypher)
    #             print("records:"+str(records[i]))
    #             print("optimized_records:"+str(optimized_records[i]))
    #         f.write(str(records[i])+"\n")
    #         f.write(str(optimized_records[i])+"\n")
    # f.write("\n")

def convert_to_number(s):
    if s.isdigit():
        return int(s)
    else:
        try:
            return float(s)
        except ValueError:
            return s

def OptTest(folder_name):
    # print("folder name:", folder_name)
    # print("graph:", graph)
    graph_folder=os.path.join(root_folder,folder_name)
    cypher_folder = os.path.join(graph_folder,"WriteQueries")
    if(os.path.exists(cypher_folder)==False):
        return
    parameter_folder=os.path.join(graph_folder,"parameter")
    original_result_map=dict()
    optimized_result_map=dict()
    for i in range(0,cycle):
        # 测试
        for file in sorted(os.listdir(cypher_folder)):
            with open(os.path.join(cypher_folder,file)) as f:
                cypher_name=file.split(".")[0]
                input_cypher=f.read()
                parameter_path=os.path.join(parameter_folder,cypher_name+".json")
                parameters=[]
                if os.path.exists(parameter_path):
                    f2 = open(parameter_path,"r")
                    data = json.load(f2)
                    for key, value in data.items():
                        input_cypher = input_cypher.replace(key, str(value))
                    # parameter_lines=f2.readlines()
                    # for line in parameter_lines:
                    #     parameters.append(convert_to_number(line.strip()))
                print(input_cypher)
                # if len(parameters)>0:
                #     input_cypher=input_cypher % tuple(parameters)
                # print(input_cypher)
                original_time,optimized_time=test_cypher(input_cypher)
                if input_cypher in original_result_map:
                    original_result_map[input_cypher]+=original_time
                else:
                    original_result_map[input_cypher]=original_time
                if input_cypher in optimized_result_map:
                    optimized_result_map[input_cypher]+=optimized_time
                else:
                    optimized_result_map[input_cypher]=optimized_time
        # recover
        view_folder=os.path.join(folder_name,"recover")
        for file in os.listdir(view_folder):       
            lines = open(view_folder+"/"+file).readlines()
            for line in lines:
                print(line)
                test_cypher(line)

    f=open(output_path,"a")
    for input_cypher, values in original_result_map.items():
        f.write(input_cypher+"\n")
        f.write("cycle: "+str(cycle)+"\n")
        f.write("original_time: "+str(original_result_map[input_cypher]/cycle)+"\n")
        f.write("optimized_time: "+str(optimized_result_map[input_cypher]/cycle)+"\n")
        f.write("speed up: "+str(original_result_map[input_cypher]/optimized_result_map[input_cypher])+"\n")
        f.write("\n")


if  __name__ == '__main__':
    parse_args()
    if folder_name=='':
        folder_name=graph
    global output_path
    output_path=os.path.join(root_folder,folder_name,"maintenance_log.txt")
    OptTest(folder_name)
    