from TuGraphClient import TuGraphClient
tugraph_url = '127.0.0.1:7073'
tugraph_user = 'admin'
tugraph_password = '73@TuGraph'
tugraph_graph = 'ldbcSf1'
cypher="Optimize match (n0:Comment)-[r1:replyOf*..]->(n1:Post)<-[r2:replyOf*..]-(n2:Comment) where n0<>n2 return count(n1)"
def listtostr(listcypher):
    result=""
    for its in listcypher:
        for it in its: 
            result+=it
    return result
if __name__=="__main__":
    client = TuGraphClient(tugraph_url,tugraph_user,tugraph_password,tugraph_graph)
    opt_cypher_list=client.call_cypher(cypher)["result"]
    opt_cypher=listtostr(opt_cypher_list)
    print(type(opt_cypher))
