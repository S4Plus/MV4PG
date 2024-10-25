MATCH (acc:Account {id: $accountId})-[r:repay{timestamp: $repayTime}]->(loan: Loan {id: $loanId})
delete r