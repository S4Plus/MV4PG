MATCH (src:molecule)-[:similarity]->(:molecule)-[:transform]->(dst:molecule)
RETURN count(*) AS count