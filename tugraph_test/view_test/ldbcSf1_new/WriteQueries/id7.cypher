MATCH (:Comment {id: $commentId})<-[:replyOf*0..]-(comment:Comment)
DETACH DELETE comment
