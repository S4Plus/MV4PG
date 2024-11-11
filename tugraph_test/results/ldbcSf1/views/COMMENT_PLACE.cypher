create view COMMENT_PLACE as 
( Construct (n)-[r:COMMENT_PLACE]->(m) 
match (m:Place)<-[r1*2..2]-(n:Comment) )