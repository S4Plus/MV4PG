MATCH (fg:functional_group {id:'$fgId'})<-[:molecule_functional_group]-(:molecule)-[:molecule_template]->(:reaction_template)-[:template_product]->(p:molecule)
RETURN count(*) AS count