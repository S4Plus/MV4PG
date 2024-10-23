 MATCH (n:Place{name:"Taiwan"}) set n.type='city' return n
 MATCH (n:Place{name:"Taiwan"}) set n.isPartOf=1 return n
MATCH (n:Place{name:"Hong_Kong"}) set n.type='city' return n
MATCH (n:Place{name:"Hong_Kong"}) set n.isPartOf=1 return n
 MATCH (n:Place{name:"Taiwan"})-[r:isPartOf]->(m) with r delete r
 MATCH (n:Place{name:"Taiwan"}),(m:Place{name:"China"}) create (n)-[r:isPartOf]->(m)
 MATCH (n:Place{name:"Hong_Kong"})-[r:isPartOf]->(m) with r delete r
 MATCH (n:Place{name:"Hong_Kong"}),(m:Place{name:"China"}) create (n)-[r:isPartOf]->(m)