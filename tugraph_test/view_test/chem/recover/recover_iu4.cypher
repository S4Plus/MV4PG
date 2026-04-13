MATCH (mid:molecule {id: 31})-[r:transform]->(dst:molecule {id: 2})
WHERE r.template_id='BENCH_RT' AND r.confidence=0.1234
DELETE r