create view FG_TEMPLATE_PRODUCT as
( Construct (fg)-[r:FG_TEMPLATE_PRODUCT]->(p)
match (fg:functional_group)<-[:molecule_functional_group]-(:molecule)-[:molecule_template]->(:reaction_template)-[:template_product]->(p:molecule) )