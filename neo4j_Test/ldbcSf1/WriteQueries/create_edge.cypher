MATCH (p1:Person {id:"987"}), (p2:Person {id:"2199023265994"})
CREATE (p1)-[:knows {creationDate:"13824310250", weight:"0”}]->(p2)
