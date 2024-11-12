match (n0:Tag)<-[r1:hasInterest]-(n1:Person)
match (n1)-[r2]->(n2)-[r3]->(n3:Place) where n0.hasType>$tagType return count(n3)