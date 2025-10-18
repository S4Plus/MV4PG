MATCH
  (author:Person {id: $authorPersonId}),
  (country:Place {id: $countryId}),
  (message:Post {id: $replyToPostId})
CREATE (author)<-[:commentHasCreator{creationDate:$creationDate}]-(c:Comment {
    id: $commentId,
    creationDate: $creationDate,
    locationIP: $locationIP,
    browserUsed: $browserUsed,
    content: $content,
    length: $length,
    creator: $authorPersonId,
    place: $countryId
  })-[:replyOf{creationDate:$creationDate}]->(message),
  (c)-[:commentIsLocatedIn{creationDate:$creationDate}]->(country)
WITH c
UNWIND $tagIds AS tagId
  MATCH (t:Tag {id: tagId})
  CREATE (c)-[:commentHasTag]->(t)
