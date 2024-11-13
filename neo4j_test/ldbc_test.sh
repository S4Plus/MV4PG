path=./ldbcSf1
neo4j_url1="bolt://localhost:7690" 
neo4j_user1="neo4j"
neo4j_password1="123456"
neo4j_url2="bolt://localhost:7691" 
neo4j_user2="neo4j"
neo4j_password2="352541141"
tugraph_url="127.0.0.1:7072"
tugraph_user='admin'
tugraph_password='73@TuGraph'
tugraph='ldbcSf1'
need_init=$1
docker exec neo4j1 bash -c "cat > /var/lib/neo4j/conf/neo4j.conf << EOL
dbms.tx_log.rotation.retention_policy=100M size
dbms.memory.heap.initial_size=63G
dbms.memory.heap.max_size=63G
dbms.tx_state.memory_allocation=ON_HEAP
dbms.memory.pagecache.size=512M
dbms.connector.http.listen_address=:7443
dbms.connector.bolt.listen_address=:7690
dbms.default_listen_address=0.0.0.0
dbms.security.auth_enabled=true
cypher.lenient_create_relationship = true
dbms.default_database=ldbcSf1
dbms.directories.logs=/logs
EOL"
docker exec neo4j2 bash -c "cat > /var/lib/neo4j/conf/neo4j.conf << EOL
dbms.tx_log.rotation.retention_policy=100M size
dbms.memory.heap.initial_size=63G
dbms.memory.heap.max_size=63G
dbms.tx_state.memory_allocation=ON_HEAP
dbms.memory.pagecache.size=512M
dbms.connector.http.listen_address=:7444
dbms.connector.bolt.listen_address=:7691
dbms.default_listen_address=0.0.0.0
dbms.security.auth_enabled=true
cypher.lenient_create_relationship = true
dbms.default_database=ldbcSf1
dbms.directories.logs=/logs
EOL"
docker restart neo4j1
docker restart neo4j2
cd ./CypherRewrite/build
./CypherRewrite $path
echo "complete rewrite"
cd ../..
if [ "$need_init" = true ]; then
    python create_views.py -path $path
fi
echo "complete init"
python new_run.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2
echo "complete run"