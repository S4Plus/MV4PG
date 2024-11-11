MATCH (src:Account{id:%d})<-[e1:transfer]-(m:Account) -[e2:transfer]->(dst:Account) 
WHERE dst.isBlocked = true AND src.id <> dst.id AND e1.timestamp > %d AND e1.timestamp < %d AND e2.timestamp > %d AND e2.timestamp < %d 
RETURN DISTINCT dst.id as dstId ORDER BY dstId ASC;