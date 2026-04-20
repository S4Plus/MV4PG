create (n:molecule{id:'9',SMILES:'FF',formula:'F2',ismiles:'[F:1][F:2]'})

match (n:molecule{id:'9'}),(m:molecule{id:'4'}) create (n)<-[r:replacement_edges{functional_group:'[*]F',functional_group_formula:'F',replace_atom:1}]-(m)

match (n:molecule{id:'9'}),(m:formula{id:'formula::F2'}) create (n)-[r:molecule_formula]->(m)

match (n:molecule{id:'9'}),(m:functional_group{id:'fg::[*]F'}) create (n)-[r:molecule_functional_group{count:2,positions:'0;1'}]->(m)