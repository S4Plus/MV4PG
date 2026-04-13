create view TEMPLATE_PRODUCT_CANDIDATE as
( Construct (m)-[r:TEMPLATE_PRODUCT_CANDIDATE]->(p)
match (m:molecule)-[:molecule_template]->(t:reaction_template)-[:template_product]->(p:molecule) )