MATCH
  (tag:Tag {name: "Rumi"}),
  (person1:Person)<-[:commentHasCreator]-(message1:Comment)-[:replyOf*1..]->(post1:Post)<-[:containerOf]-(forum1:Forum),
  (message1)-[:commentHasTag]->(tag),
  (forum1)<-[:hasMember]->(person2:Person)<-[:commentHasCreator]-(comment:Comment)-[:HAS_TAG]->(tag),
  (forum1)<-[:hasMember]->(person3:Person)<-[:commentHasCreator]-(message2:Comment),
  (comment)-[:replyOf]->(message2)-[:replyOf*1..]->(post2:Post)<-[:containerOf]-(forum2:Forum)
	,(comment)-[:commentHasTag]->(tag)
	, (message2)-[:commentHasTag]->(tag)
WHERE forum1 <> forum2
  AND message2.creationDate > message1.creationDate 
  
RETURN person1.id, count(DISTINCT message2) AS messageCount
ORDER BY messageCount DESC, person1.id ASC
LIMIT 10