create view ROOT_POST as 
( Construct (n)-[r:ROOT_POST]->(m) 
match (n:Comment)-[r1:replyOf*..]->(m:Post) )