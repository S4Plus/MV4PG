MATCH (tag:Tag)<-[:postHasTag]-(message:Post)<-[:replyOf]-(comment:Comment) 
RETURN count(*) AS count
