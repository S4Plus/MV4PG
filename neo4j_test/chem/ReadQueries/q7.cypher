MATCH (n:molecule)-[]->(:molecule)-[:transform*..]->(m:molecule)
RETURN count(*) AS count