match (n0:Comment)-[r1:replyOf*..]->(n1:Post) 
match (n1)<-[r2:replyOf*..]-(n2:Comment) where n0<>n2 return count(n1)