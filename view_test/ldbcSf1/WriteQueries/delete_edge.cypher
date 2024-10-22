MATCH (n:Person {id: $personId})-[r:likes]->(m:Comment {id: $commentId})
DELETE r