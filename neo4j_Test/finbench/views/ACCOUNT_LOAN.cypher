create view ACCOUNT_LOAN as (
    Construct (n)-[r:ACCOUNT_LOAN]->(m)
    match (n:Account)-[r1*..2]->(m:Loan)
)