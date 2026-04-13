MATCH (src:molecule {id: 1})-[r:replacement_edges]->(dst:molecule {id: 2})
WHERE r.functional_group='__bench_fg__' AND r.functional_group_formula='__bench_formula__' AND r.replace_atom=999
DELETE r