create view COMMENT_TAG as 
( Construct (n)-[r:COMMENT_TAG]->(m) 
match (n:Comment)-[:replyOf]->(:Post)-[:postHasTag]->(m:Tag) )