create (n:Loan{id:569547023196251,createTime:"1578056331433",balance:49222390,interestRate:"0.044",loanAmount:49222390,loanUsage:"vacations"})
MATCH (n:Loan {id:569547023196251}),(m:Company{id:298213}) create (n)<-[r:apply{timestamp:1578056331433,org:"Upgrade"}]-(m)
MATCH (n:Loan {id:569547023196251}),(m:Account{id:176209932490651550}) create (n)-[r:deposit{timestamp:1666883006453,amount:7984070.02}]->(m)
MATCH (n:Loan {id:569547023196251}),(m:Account{id:176209932490651550}) create (n)<-[r:repay{timestamp:1667848081686,amount:8394244.26}]-(m)
MATCH (n:Loan {id:569547023196251}),(m:Account{id:176209932490651550}) create (n)<-[r:repay{timestamp:1655888588951,amount:32315623.19}]-(m)
MATCH (n:Loan {id:569547023196251}),(m:Account{id:208016604858955680}) create (n)<-[r:repay{timestamp:1669430496462,amount:952660.18}]-(m)
MATCH (n:Loan {id:569547023196251}),(m:Account{id:208016604858955680}) create (n)<-[r:repay{timestamp:1644281458549,amount:3059914.45}]-(m)