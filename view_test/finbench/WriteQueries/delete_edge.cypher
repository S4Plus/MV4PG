MATCH (acc:Account {id: $accountId})-[r:repay]->(loan: Loan {id: $loanId}) where r.timestamp=$repayTime
delete r