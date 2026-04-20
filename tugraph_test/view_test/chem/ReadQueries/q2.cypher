MATCH (n:molecule)-[:replacement_edges*..3]->(m:molecule)
RETURN count(*) AS count