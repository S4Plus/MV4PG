match (n0:Comment)-[r1*2..2]->(n1:Place)<-[r2:isPartOf]-(n2:Place) return count(n2)