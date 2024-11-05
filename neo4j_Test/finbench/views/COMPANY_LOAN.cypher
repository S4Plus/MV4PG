create view COMPANY_LOAN as (
    Construct (n)-[r:COMPANY_LOAN]->(m)
    match (n:Company)-[r1]->()-[r2]->(m:Loan)
)