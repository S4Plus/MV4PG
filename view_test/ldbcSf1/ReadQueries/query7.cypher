match (n0:Comment)-[r1:replyOf*..]->(n1:Post)
match (n1)-[r2:postHasCreator]->(n2:Person)
match (n2)-[r3]->(n3)-[r4]->(n4:Place) where n0.creationDate<$creationDate return count(n4)