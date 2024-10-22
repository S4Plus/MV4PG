MATCH (dstCard:Account {id:%d} )<-[edge2:withdraw]-(mid:Account) 
WHERE dstCard.type ENDS WITH 'card' AND edge2.timestamp > %d AND edge2.timestamp < %d 
AND edge2.amount > %f 
WITH mid, sum(edge2.amount) as sumEdge2Amount, count(edge2.amount) as t 
MATCH (mid)<-[edge1:transfer]-(src:Account) 
WHERE edge1.timestamp > %d AND edge1.timestamp < %d AND edge1.amount > %f 
WITH mid.id AS midId, count(edge1) AS edge1Count, sum(edge1.amount) AS sumEdge1Amount, sumEdge2Amount 
WHERE edge1Count > 3 
WITH midId, sumEdge1Amount, sumEdge2Amount 
RETURN midId, round(sumEdge1Amount * 1000) / 1000 as sumEdge1Amount, round(sumEdge2Amount * 1000) / 1000 as sumEdge2Amount 
ORDER BY sumEdge2Amount DESC;