match (n0:Person)-[r1*2]->(n1:Company)
match (n1)<-[r2*2]-(n2:Person) where n0<>n2 return count(n1)