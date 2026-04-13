MATCH (src:molecule {id: $srcId}), (mid:molecule {id: $midId})
CREATE (src)-[:similarity {similarity:0.000123}]->(mid)