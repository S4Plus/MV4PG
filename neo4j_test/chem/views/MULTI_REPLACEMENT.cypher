create view MULTI_REPLACEMENT as
( Construct (n)-[r:MULTI_REPLACEMENT]->(m)
match (n:molecule)-[:replacement_edges*..3]->(m:molecule) )