MATCH (person:Person)-[:knows*..$num]-(friend:Person) with distinct friend match (friend:Person)<-[:postHasCreator]-(post:Post)-[:postHasTag]->(t:Tag)
return count(*)