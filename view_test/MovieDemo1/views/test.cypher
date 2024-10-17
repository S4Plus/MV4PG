create view test as (
  Construct (n)-[r:test]->(m)
  match (n:user)-[:rate]->(:movie)-[:has_keyword]->(m:keyword)
)