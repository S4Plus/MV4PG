create view REPLACEMENT_ANCESTOR_SCAFFOLD as
( Construct (src)-[r:REPLACEMENT_ANCESTOR_SCAFFOLD]->(ancestor)
match (src:molecule)-[:replacement_edges]->(:molecule)-[:molecule_scaffold]->(:scaffold)-[:scaffold_generalization*..]->(ancestor:scaffold) )