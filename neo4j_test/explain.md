# 使用说明
- neo4j服务器在docker:wxdneo4j1和wxdneo4j2
- 测试snb 执行ldbc_test.sh 需要修改其中path路径为ldbcSf1的绝对路径 若要创建视图执行 ldbc_test.sh true
- 测试finbench 同理
# result
对应的测试结果保存在path/result下
- oldtime.json 未优化的读语句测试结果
- opttime.json 优化的读语句测试结果
- oldwritetime.json 未优化的写语句测试结果
- optwritetime.json 优化后的写语句测试结果
- all_writetime.json 未优化的五次写语句测试结果
- all_opt_writetime.json 优化后的五次写语句测试结果
- createviews.json 视图创建测试结果
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