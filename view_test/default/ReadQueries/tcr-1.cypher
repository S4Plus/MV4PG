MATCH p = (acc:Account {id:%d})-[e1:transfer *1..3]->(other:Account)<-[e2:signIn]-(medium) 
WHERE e2.timestamp > %d AND e2.timestamp < %d 
AND medium.isBlocked = true 
RETURN DISTINCT other.id as otherId, medium.id as mediumId, medium.type as mediumType 
ORDER BY otherId, mediumId;