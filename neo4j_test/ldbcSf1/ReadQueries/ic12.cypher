MATCH (:Person)-[:knows]-(friend:Person)<-[:commentHasCreator]-(comment:Comment)-[:replyOf]->(:Post)-[:postHasTag]->(tag:Tag)
RETURN count(*)
