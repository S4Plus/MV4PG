MATCH (m:molecule {id: 1})-[r:molecule_template]->(t:reaction_template {id:'RT03'})
WHERE r.role='bench_role'
DELETE r