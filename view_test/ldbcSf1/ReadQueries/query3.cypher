match (n0:Place)<-[r1:personIsLocatedIn]-(n1:Person)
match (n1)-[r2]->(n2)-[r3]->(n3:Place) return count(n3)