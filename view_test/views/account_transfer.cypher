create view AccountTransfer as (
    Construct (n)-[r:AccountTransfer]->(m)
    match (n:Account)-[r:transfer *1..3]->(m:Account)
)