MATCH (loan:Loan {id: $loanId})
DETACH DELETE loan