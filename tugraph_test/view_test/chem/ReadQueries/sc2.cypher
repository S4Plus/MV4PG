MATCH (m:molecule)-[:molecule_scaffold]->(:scaffold)-[:scaffold_generalization*..]->(ancestor:scaffold {id:'$ancestorScaffoldId'})
RETURN count(*) AS count