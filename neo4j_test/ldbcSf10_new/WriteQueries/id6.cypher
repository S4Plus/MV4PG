MATCH (:Post {id:"8796133261056"})<-[:replyOf*0..]-(message) detach delete message
