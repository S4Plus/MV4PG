create view MOLECULE_ANCESTOR_SCAFFOLD as
( Construct (m)-[r:MOLECULE_ANCESTOR_SCAFFOLD]->(ancestor)
match (m:molecule)-[:molecule_scaffold]->(:scaffold)-[:scaffold_generalization*..]->(ancestor:scaffold) )