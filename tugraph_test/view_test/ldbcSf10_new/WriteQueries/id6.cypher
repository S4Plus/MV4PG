MATCH (:Post {id: $postId})<-[:replyOf*0..]-(message) 
DETACH DELETE message
