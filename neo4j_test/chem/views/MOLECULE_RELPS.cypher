create view MOLECULE_RELPS as
( Construct (n)-[r:MOLECULE_RELPS]->(m)
MATCH (n:molecule)-[:transform]->(:molecule)-[:replacement_edges]->(m:molecule) )