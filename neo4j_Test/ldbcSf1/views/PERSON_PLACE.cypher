create view PERSON_PLACE as 
( Construct (n)-[r:PERSON_PLACE]->(m) 
MATCH (n:Person)-[r1]->()-[r2]->(m:Place))