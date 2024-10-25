match (n0:Person)-[r1*2]->(n1:Company)
match (n0)-[r2:own]->(n2:Account) return count(n1)