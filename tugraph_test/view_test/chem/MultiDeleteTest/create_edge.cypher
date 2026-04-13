MATCH (src:molecule {id: $bulkSrcId}), (dst:molecule {id: $bulkDstId})
CREATE (src)-[:transform {template_id:'BULK_BENCH_RT', confidence:0.4321}]->(dst)