MATCH (n:molecule)-[:transform]->(:molecule)-[:replacement_edges]->(m:molecule)
RETURN count(*) AS count
