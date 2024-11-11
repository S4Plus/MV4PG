match (n0:Medium)-[r1:signIn]->(n1:Account)
match (n1)-[r2*..2]->(n2:Loan) return count(n2)