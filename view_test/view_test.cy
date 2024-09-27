match (n)-[r]->(m) return count(r)
match (n:person{id:5}),(m:movie{id:28}) create (n)-[r:write]->(m)
match (n:person{id:5}),(m:movie{id:28}) create (n)-[r:write]->(m)
match (n:person{id:5})-[r NoDupEdge]->(m:movie{id:28}) delete r