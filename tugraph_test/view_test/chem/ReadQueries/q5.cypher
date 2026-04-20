MATCH (n:formula)<-[:molecule_formula]-(:molecule)-[]->(:molecule)-[:transform*..]->(m:molecule)
RETURN count(*) AS count
