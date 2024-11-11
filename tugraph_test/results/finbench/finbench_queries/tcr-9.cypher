MATCH (mid:Account {id:%d}) 
WITH mid 
OPTIONAL MATCH (mid)-[edge2:repay]->(loan:Loan) 
WHERE edge2.amount > %f AND edge2.timestamp > %d AND edge2.timestamp < %d 
WITH mid, sum(edge2.amount) AS edge2Amount 
OPTIONAL MATCH (mid)<-[edge1:deposit]-(loan:Loan) 
WHERE edge1.amount > %f AND edge1.timestamp > %d AND edge1.timestamp < %d 
WITH mid, sum(edge1.amount) AS edge1Amount, edge2Amount 
OPTIONAL MATCH (mid)-[edge4:transfer]->(down:Account) 
WHERE edge4.amount > %f AND edge4.timestamp > %d AND edge4.timestamp < %d 
WITH mid, edge1Amount, edge2Amount, sum(edge4.amount) AS edge4Amount 
OPTIONAL MATCH (mid)<-[edge3:transfer]-(up:Account) 
WHERE edge3.amount > %f AND edge3.timestamp > %d AND edge3.timestamp < %d 
WITH edge1Amount, edge2Amount, sum(edge3.amount) AS edge3Amount, edge4Amount 
RETURN CASE WHEN edge2Amount=0 THEN -1 ELSE round(1000.0 * edge1Amount / edge2Amount) / 1000 END AS ratioRepay, 
CASE WHEN edge4Amount=0 THEN -1 ELSE round(1000.0 * edge1Amount / edge4Amount) / 1000 END AS ratioDeposit, 
CASE WHEN edge4Amount=0 THEN -1 ELSE round(1000.0 * edge3Amount / edge4Amount) / 1000 END AS ratioTransfer;