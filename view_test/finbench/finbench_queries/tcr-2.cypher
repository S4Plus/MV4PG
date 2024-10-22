MATCH (p:Person {id:%d})-[e1:own]->(acc:Account) <-[e2:transfer*1..3]-(other:Account)
 WITH DISTINCT other MATCH (other)<-[e3:deposit]-(loan:Loan) 
 WHERE e3.timestamp > %d AND e3.timestamp < %d 
 WITH DISTINCT other.id AS otherId, loan.loanAmount AS loanAmount, loan.balance AS loanBalance 
 WITH otherId AS otherId, sum(loanAmount) as sumLoanAmount, sum(loanBalance) as sumLoanBalance 
 RETURN otherId, round(sumLoanAmount * 1000) / 1000 as sumLoanAmount, round(sumLoanBalance * 1000) / 1000 as sumLoanBalance 
 ORDER BY sumLoanAmount DESC, otherId ASC;