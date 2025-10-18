MATCH (country:Place{id:'54'})
MATCH (person1:Person)-[:personIsLocatedIn]->(city1:Place)-[:isPartOf]->(country)
MATCH (person2:Person)-[:personIsLocatedIn]->(city2:Place)-[:isPartOf]->(country)
MATCH (person3:Person)-[:personIsLocatedIn]->(city3:Place)-[:isPartOf]->(country)
RETURN count(*) AS count
