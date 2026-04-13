MATCH (src:molecule {id: 31})-[r:transform]->(dst:molecule {id: 2})
WHERE r.template_id='BULK_BENCH_RT' AND r.confidence=0.4321
DELETE r