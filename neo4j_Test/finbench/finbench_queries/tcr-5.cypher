MATCH (person:Person {id:%d})-[e1:own]->(src:Account) 
WITH src 
MATCH p=(src)-[e2:transfer*1..3]->(dst:Account) 
return count(p)