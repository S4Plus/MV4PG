import sys

is_delete=False
if(len(sys.argv)>1):
    if(sys.argv[1]=="True" or sys.argv[1]=="true"):
        is_delete=True
delete_cypher="CALL db.deleteLabel('edge', 'ROOT_POST')"
cypher='create view ROOT_POST as match (n:Comment)-[:replyOf*..]->(m:Post) return n,m '
print(bool(is_delete))