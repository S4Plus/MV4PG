MATCH (message:Comment)-[:replyOf*..]->(post:Post)-[:postHasCreator]->(person)
RETURN count(*)