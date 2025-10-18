match (n:Person)<-[r1:postHasCreator]-(p:Post)-[r2:postHasTag]->(m:Tag) 
return count(*)