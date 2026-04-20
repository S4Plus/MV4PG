MATCH (n:molecule)-[:transform*..]->(:molecule)-[:replacement_edges*..3]->(m:molecule)
RETURN count(*) AS count
