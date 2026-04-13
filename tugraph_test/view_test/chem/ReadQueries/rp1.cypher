MATCH (src:molecule)-[:replacement_edges]->(:molecule)-[:molecule_scaffold]->(:scaffold)-[:scaffold_generalization*..]->(ancestor:scaffold)
RETURN count(*) AS count