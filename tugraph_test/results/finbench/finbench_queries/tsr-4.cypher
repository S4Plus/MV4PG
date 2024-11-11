MATCH (n:Account{id:%d}) 
WITH n 
MATCH (n)-[e:transfer]->(m:Account) 
WHERE e.amount > %f AND e.timestamp > %d AND e.timestamp < %d 
WITH m.id as dstId, count(e) as numEdges, sum(e.amount) as sumAmount 
RETURN dstId, numEdges, round(sumAmount * 1000) / 1000 as sumAmount ORDER BY sumAmount DESC, dstId ASC;