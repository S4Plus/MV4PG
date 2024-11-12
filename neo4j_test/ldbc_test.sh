path=/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/ldbcSf1
neo4j_url1="bolt://localhost:7690" 
neo4j_user1="neo4j"
neo4j_password1="123456"
neo4j_url2="bolt://localhost:7691" 
neo4j_user2="neo4j"
neo4j_password2="352541141"
tugraph_url="127.0.0.1:7073"
tugraph_user='admin'
tugraph_password='73@TuGraph'
tugraph='ldbcSf1'
need_init=$1
if [ "$need_init" = true ]; then
    python create_views.py -path $path
fi
echo "初始化完成"
python new_run.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2