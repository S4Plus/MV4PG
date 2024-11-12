match (n:Comment{id:"2199029813921"})-[r:replyOf]->(m:Post{id:"2199029813920"}) where r.creationDate="1700000000000" delete r
