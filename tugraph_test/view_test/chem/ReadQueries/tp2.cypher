MATCH (m:molecule)-[:molecule_template]->(:reaction_template)-[:template_product]->(p:molecule)-[:molecule_scaffold]->(s:scaffold {id:'$scaffoldId'})
RETURN count(*) AS count