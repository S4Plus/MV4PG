MATCH (:Comment {id:"8796133547179"})<-[:replyOf*0..]-(comment:Comment) detach delete comment
