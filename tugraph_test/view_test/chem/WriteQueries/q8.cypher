match (n:molecule{id:$srcID})-[r:replacement_edges]->(m:molecule{id:$dstID})
DELETE r