match (n0:Place)<-[r1]-(n1)<-[r2]-(n2:Person)-[r3:personIsLocatedIn]->(n3:Place) return count(n3)