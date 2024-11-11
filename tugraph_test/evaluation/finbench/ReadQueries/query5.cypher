match (n0:Loan)<-[r1]-(n1)<-[r2]-(n2:Company)
match (n2)<-[r3*2]-(n3:Person) return count(n3)