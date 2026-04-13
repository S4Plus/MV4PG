MATCH (mid:molecule {id: $midId}), (dst:molecule {id: $dstId})
CREATE (mid)-[:transform {template_id:'BENCH_RT', confidence:0.1234}]->(dst)