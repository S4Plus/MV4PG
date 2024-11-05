create view PERSON_COMPANY as (
    Construct (n)-[r:PERSON_COMPANY]->(m)
    match (n:Person)-[r1*2]->(m:Company)
)