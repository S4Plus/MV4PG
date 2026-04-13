create view SIMILAR_TRANSFORM as
( Construct (src)-[r:SIMILAR_TRANSFORM]->(dst)
match (src:molecule)-[:similarity]->(:molecule)-[:transform]->(dst:molecule) )