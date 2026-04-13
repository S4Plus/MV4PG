MATCH (m:molecule {id: $moleculeId}), (t:reaction_template {id:'$templateId'})
CREATE (m)-[:molecule_template {role:'bench_role'}]->(t)