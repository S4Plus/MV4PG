create view SameTransferSource as (
    Construct (n)-[r:SameTransferSource]->(m)
    match (n:Account)<-[:transfer]-(:Account)-[:transfer]->(m:Account)
)