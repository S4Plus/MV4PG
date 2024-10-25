# neo4j触发器设置

使用脚本链接neo4j服务器设置视图维护触发器，以及新的tugrph维护语句逻辑

# 使用说明

- 在tugraph-compiler环境中，clone项目。
- 新建build文件夹，执行cmake ..以及make，得到两个可执行文件一个为CypherRewrite，以及CypherRewriteneo4j。CypherRewriteneo4j执行需要一个test_input.txt,其中放置需要创立的视图语句，通过运行之后会生成output.json
其中放置了所有视图的视图维护语句.
## json
{
    "match (n:Comment)-[:replyOf*..]->(m:Post) WITH n,m CREATE (n)-[r:ROOT_POST]->(m)": [
        [
            "match (n:Comment)-[*0]-(viewvetex)-[:replyOf*..]->(m:Post) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]",
            "match (n:Comment)-[:replyOf*..]->(m:Post)-[*0]-(viewvetex) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]",
            "match (n:Comment)-[:replyOf*1..]->(viewvetex)->[:replyOf*0..]->(m:Post) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]"
        ],
        [
            "match (n:Comment)-[:replyOf*0..]->()-[viewedge:replyOf]->()-[:replyOf*0..]->(m:Post) WITH n,m CREATE (n)-[r:ROOT_POST]->(m)"
        ],
        [
            "match (n:Comment)-[:replyOf*0..]->()-[viewedge:replyOf]->()-[:replyOf*0..]->(m:Post) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]"
        ]
    ]
}

- 其中key为视图创建语句，val保存的依次是其key对应视图删除点，增加边，删除边的视图维护语句。
- 在生成json文件之后，运行neo4j_neo4j_trigger.py 
此脚本获得json文件中的keys，并且链接neo4j，执行视图创建，并且获得json的vals，借此生成其对应的触发器生成语句，并交给neo4j执行。

# 创建视图语句
```
match (n:Comment)-[:replyOf*1..]->(m:Post) WITH n,m CREATE (n)-[r:ROOT_POST]->(m)
```
# 维护语句
## 删除点
```
 "match (n:Comment)-[*0]-(viewvetex)-[:replyOf*1..]->(m:Post) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]"

 "match (n:Comment)-[:replyOf*1..]->(m:Post)-[*0]-(viewvetex) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]"

 "match (n:Comment)-[:replyOf*1..]->(viewvetex)->[:replyOf*0..]->(m:Post) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]"
```
## 创建边
```
 "match (n:Comment)-[:replyOf*0..]->()-[viewedge:replyOf]->()-[:replyOf*0..]->(m:Post) WITH n,m CREATE (n)-[r:ROOT_POST]->(m)"
```
## 删除边
```
 "match (n:Comment)-[:replyOf*0..]->()-[viewedge:replyOf]->()-[:replyOf*0..]->(m:Post) WITH n,m match (n)-[r:ROOT_POST]->(m) WITH n,m,collect(r) as view delete view[0]"
```
# 测试语句
删除后再创建观察视图边的个数是否不变
```
delete_cypher ='match (n:Comment{id:558})-[r:replyOf]->(m:Post{id:556}) with r limit 1 delete r'
create_cypher ='match (n:Comment{id:558}),(m:Post{id:556}) with n,m create (n)-[:replyOf{creationDate:1266604713724}]->(m)'
```