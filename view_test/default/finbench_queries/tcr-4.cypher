MATCH (src:Account {id:%d})-[e1:transfer]->(dst:Account {id:%d}) 
WHERE e1.timestamp > %d AND e1.timestamp < %d 
WITH src, dst.id as dstid 
MATCH (src)<-[e2:transfer]-(other:Account)<-[e3:transfer]-(dst:Account) 
WHERE dst.id=dstid AND e2.timestamp > %d AND e2.timestamp < %d 
AND e3.timestamp > %d AND e3.timestamp < %d 
WITH DISTINCT src, other, dst 
MATCH (src)<-[e2:transfer]-(other) 
WHERE e2.timestamp > %d AND e2.timestamp < %d 
WITH src, other, dst, count(e2) as numEdge2, sum(e2.amount) as sumEdge2Amount, max(e2.amount) as maxEdge2Amount 
MATCH (other)<-[e3:transfer]-(dst) 
WHERE e3.timestamp > %d AND e3.timestamp < %d 
WITH other.id as otherId, numEdge2, sumEdge2Amount, maxEdge2Amount, count(e3) as numEdge3, sum(e3.amount) as sumEdge3Amount, max(e3.amount) as maxEdge3Amount 
RETURN otherId, numEdge2, round(sumEdge2Amount * 1000) / 1000 as sumEdge2Amount, round(maxEdge2Amount * 1000) / 1000 as maxEdge2Amount, numEdge3, round(sumEdge3Amount * 1000) / 1000 as sumEdge3Amount, round(maxEdge3Amount * 1000) / 1000 as maxEdge3Amount ORDER BY sumEdge2Amount DESC, sumEdge3Amount DESC, otherId ASC;