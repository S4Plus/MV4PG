import json
def filter(profile):
    result={}
    db=0
    dbHits=profile.get("dbHits")
    print(dbHits)
    if dbHits is not None:
        db=dbHits
    rows=profile.get("rows")
    typee=profile.get("operatorType")
    detail=profile["Details"]
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
def main():
    with open('/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/ldbcSf1/result/opttime.json', 'r', encoding='utf-8') as filter_file:
        pat = json.load(filter_file)
        keys=pat.keys()
        for key in keys:
            profile=pat[key]["profile"]
            dbs,profile=filter(profile)
            pat[key]["AllDbHits"]=dbs    
            pat[key]["profile"]=profile
        with open("./new.json",'w') as outfile:
            json.dump(pat,outfile,indent=4) 
main()