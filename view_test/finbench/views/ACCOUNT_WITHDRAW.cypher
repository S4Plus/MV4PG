create view ACCOUNT_WITHDRAW as (
    Construct (n)-[r:ACCOUNT_WITHDRAW]->(m)
    MATCH (n:Account)-[r1:withdraw*..]->(m:Account)
)