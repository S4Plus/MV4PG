MATCH (acc:Account {id: $accountId})<-[r:deposit]-(loan: Loan {createTime: $loanCreateTime}) 
create (acc)-[:repay{timestamp:$depositTime,amount:$amt}]->(loan)