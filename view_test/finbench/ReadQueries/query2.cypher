match (n0:Company)-[r1]->(n1)-[r2]->(n2:Loan)
match (n2)<-[r3:repay]-(n3:Account) return count(n3)