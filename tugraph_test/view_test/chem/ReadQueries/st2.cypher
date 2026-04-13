MATCH (src:molecule)-[:similarity]->(:molecule)-[:transform {template_id:'$templateId'}]->(dst:molecule)
RETURN count(*) AS count