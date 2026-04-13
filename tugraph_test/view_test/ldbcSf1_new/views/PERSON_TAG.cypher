create view PERSON_TAG as 
( Construct (n)-[r:PERSON_TAG]->(m) 
match (n:Person)<-[:postHasCreator]-(:Post)-[:postHasTag]->(m:Tag) )