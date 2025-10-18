create view PERSON_PLACE as 
( Construct (n)-[r:PERSON_PLACE]->(m) 
match (n:Person)-[:personIsLocatedIn]->(:Place)-[:isPartOf]->(m:Place) )
