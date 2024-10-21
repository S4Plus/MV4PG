create view KNOWS_CREATOR as 
( Construct (n)-[r:KNOWS_CREATOR]->(m) 
  match (n:Person)-[:knows]->(:Person)<-[:commentHasCreator]-(m:Comment) 
)