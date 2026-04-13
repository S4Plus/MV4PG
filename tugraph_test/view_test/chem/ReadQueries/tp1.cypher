MATCH (m:molecule)-[:molecule_template]->(:reaction_template)-[:template_product]->(p:molecule)
RETURN count(*) AS count