create view MULTI_TRANSFORM as
( Construct (n)-[r:MULTI_TRANSFORM]->(m)
MATCH (n:molecule)-[r:transform*..]->(m:molecule) )