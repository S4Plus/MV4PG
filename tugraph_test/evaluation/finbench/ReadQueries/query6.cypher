match (n0:Account)<-[r1:own]-(n1:Company)
match (n1)-[r2]->(n2)-[r3]->(n3:Loan) return count(n3)