match (acc:Account{id:"1694622296319520"})-[r:repay]->(loan:Loan{createTime:"1656084982460"}) where r.timestamp="1729822775000" delete r
