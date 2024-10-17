create view ALL_KNOWS as 
( Construct (n)-[r:ALL_KNOWS]->(m) 
match (n:Person)-[:knows*..2]->(m:Person) )