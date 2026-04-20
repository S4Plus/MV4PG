MATCH (n:molecule{id:$srcID}),(m:molecule{id:$dstID})
CREATE (n)-[r:replacement_edges{functional_group:'test', functional_group_formula:'test', replace_atom:1}]->(m)