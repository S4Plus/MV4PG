create view Friend as (
  Construct (n)-[r:Friend]->(m)
  match (n:user)-[:is_friend*..2]->(m:user)
)