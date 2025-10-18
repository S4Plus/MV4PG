create (n:Comment{id:557,creationDate:1266580170647,locationIP:"62.93.169.198",browserUsed:"Firefox",content:"About Sarah McLachlan, f the ASPCA, as well as various other cAbout Louis I ",length:76,creator:2783,place:99,replyOfComment:0,replyOfPost:556})
create (n:Comment{id:560,creationDate:1266637836476,locationIP:"62.93.169.198",browserUsed:"Firefox",content:"ok",length:2,creator:2783,place:99,replyOfComment:557,replyOfPost:0})
create (n:Comment{id:563,creationDate:1266583605134,locationIP:"62.93.169.198",browserUsed:"Firefox",content:"yes",length:3,creator:2783,place:99,replyOfComment:557,replyOfPost:0})
create (n:Comment{id:568,creationDate:1266591466316,locationIP:"62.93.169.198",browserUsed:"Firefox",content:"ok",length:2,creator:2783,place:99,replyOfComment:557,replyOfPost:0})

Match (n:Comment{id:557}),(m:Tag{id:297}) create (n)-[r:commentHasTag]->(m)
Match (n:Comment{id:557}),(m:Tag{id:1006}) create (n)-[r:commentHasTag]->(m)
Match (n:Comment{id:557}),(m:Person{id:2783}) create (n)-[r:commentHasCreator{creationDate:1266580170647}]->(m)
Match (n:Comment{id:557}),(m:Place{id:99}) create (n)-[r:commentIsLocatedIn{creationDate:1266580170647}]->(m)
Match (n:Comment{id:557}),(m:Post{id:556}) create (n)-[r:replyOf{creationDate:1266580170647}]->(m)
Match (n:Comment{id:557}),(m:Comment{id:560}) create (n)<-[r:replyOf{creationDate:1266637836476}]-(m)
Match (n:Comment{id:557}),(m:Comment{id:563}) create (n)<-[r:replyOf{creationDate:1266583605134}]-(m)
Match (n:Comment{id:557}),(m:Comment{id:568}) create (n)<-[r:replyOf{creationDate:1266591466316}]-(m)

Match (n:Comment{id:560}),(m:Person{id:2783}) create (n)-[r:commentHasCreator{creationDate:1266637836476}]->(m)
Match (n:Comment{id:560}),(m:Place{id:99}) create (n)-[r:commentIsLocatedIn{creationDate:1266637836476}]->(m)

Match (n:Comment{id:563}),(m:Person{id:2783}) create (n)-[r:commentHasCreator{creationDate:1266583605134}]->(m)
Match (n:Comment{id:563}),(m:Place{id:99}) create (n)-[r:commentIsLocatedIn{creationDate:1266583605134}]->(m)

Match (n:Comment{id:568}),(m:Person{id:2783}) create (n)-[r:commentHasCreator{creationDate:1266591466316}]->(m)
Match (n:Comment{id:568}),(m:Place{id:99}) create (n)-[r:commentIsLocatedIn{creationDate:1266591466316}]->(m)