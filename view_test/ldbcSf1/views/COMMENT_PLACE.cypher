create view COMMENT_PLACE as 
( Construct (n)-[r:COMMENT_PLACE]->(m) 
match (n:Comment)-[r1*2..2]->(m:Place) )