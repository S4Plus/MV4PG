MATCH (n:molecule)-[:transform*..]->(m:molecule)
RETURN count(*) AS count