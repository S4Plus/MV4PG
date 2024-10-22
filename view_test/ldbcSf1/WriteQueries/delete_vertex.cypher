MATCH (n:Post {id: $postId})<-[r:replyOf*1..]-(m:Comment)
DETACH DELETE m