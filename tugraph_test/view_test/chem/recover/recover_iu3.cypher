MATCH (src:molecule {id: 1})-[r:similarity]->(mid:molecule {id: 31})
WHERE r.similarity=0.000123
DELETE r