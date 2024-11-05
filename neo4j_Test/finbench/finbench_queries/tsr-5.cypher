MATCH (n:Account{id:%d})<-[e:transfer]-(m:Account) 
WHERE e.amount > %f AND e.timestamp > %d AND e.timestamp < %d WITH m.id as srcId, count(e) as numEdges, sum(e.amount) as sumAmount 
RETURN srcId, numEdges, round(sumAmount * 1000) / 1000 as sumAmount ORDER BY sumAmount DESC, srcId ASC;