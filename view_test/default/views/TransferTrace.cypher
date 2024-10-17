create view TransferTrace as (
    Construct (n)-[r:TransferTrace]->(m)
    match (n:Account)-[r:transfer *1..3]->(m:Account)
)