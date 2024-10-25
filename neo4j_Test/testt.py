from neo4j import GraphDatabase
import json
class connector:
    def __init__(self,url,user,password):
        self.driver=GraphDatabase.driver(url,auth=(user,password))
    def close(self):
        self.driver.close()
    def run_cypher(self,query,parameters=None):
        with self.driver.session() as session:
            result=session.run(query,parameters)
            return result
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
if __name__=="__main__":
    #url="bolt://localhost:7687" 
    #user="neo4j"
    #password="352541141"
    #connector=connector(url,user,password)
    # types=["addnodes","deletenodes","addrelationship","deleterelaships"]
    path="/home/wxd/neo4j_test/graph-views/view_maintance_2024/build/output.json"
    my_tool=tool(path)
    #for view in my_tool.views:
    #     result2=connector.run_cypher(view)
    #     print(result2)
    result={}
    with open("/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/input.txt",'r') as inputfile:
        lines=inputfile.readlines()
        for line in lines:
            line=line.rstrip("\n")
            result[line]=my_tool.tran(line)
    with open('/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/new_cypher.json', 'w') as json_file:
        json.dump(result,json_file, indent=4)  # indent 参数用于美化输出