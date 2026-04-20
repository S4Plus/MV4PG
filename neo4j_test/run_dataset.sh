#!/bin/bash
# Usage: run_dataset.sh <dataset> [need_init] [test_type]
# dataset: chem | ldbcSf1 | ldbcSf10

dataset=$1
need_init=$2
test_type=${3:-0}
is_delete=${4:-false}
is_profile=${5:-false}
cycle=${6:-5}

if [ -z "$dataset" ]; then
    echo "Usage: $0 <dataset> [need_init] [test_type]"
    exit 1
fi

case $dataset in
    chem)
        path=./chem
        tugraph=chem
        database=chem
        ;;
    ldbcSf1)
        path=./ldbcSf1
        tugraph=ldbcSf1
        database=ldbcSf1
        ;;
    ldbcSf10)
        path=./ldbcSf10_new
        tugraph=ldbcSf10
        database=ldbcSf10
        ;;
    *)
        echo "Unknown dataset: $dataset"
        exit 2
        ;;
esac

neo4j_url1="bolt://localhost:7690"
neo4j_user1="neo4j"
neo4j_password1="12345678"
neo4j_url2="bolt://localhost:7691"
neo4j_user2="neo4j"
neo4j_password2="12345678"
tugraph_url="127.0.0.1:7072"
tugraph_user='admin'
tugraph_password='73@TuGraph'
docker1=neo4j1
docker1_port1=7443
docker1_port2=7690
docker2=neo4j2
docker2_port1=7444
docker2_port2=7691

echo "Configure neo4j containers for database $database"

if [ "$need_init" = true ]; then

# docker exec $docker1 bash -c "cat > /var/lib/neo4j/conf/neo4j.conf << EOL
# dbms.tx_log.rotation.retention_policy=100M size
# dbms.memory.heap.initial_size=63G
# dbms.memory.heap.max_size=63G
# dbms.tx_state.memory_allocation=ON_HEAP
# dbms.memory.pagecache.size=512M
# dbms.connector.http.listen_address=:$docker1_port1
# dbms.connector.bolt.listen_address=:$docker1_port2
# dbms.default_listen_address=0.0.0.0
# dbms.security.auth_enabled=true
# cypher.lenient_create_relationship = true
# dbms.default_database=$database
# dbms.directories.logs=/logs
# EOL"

# docker exec $docker2 bash -c "cat > /var/lib/neo4j/conf/neo4j.conf << EOL
# dbms.tx_log.rotation.retention_policy=100M size
# dbms.memory.heap.initial_size=63G
# dbms.memory.heap.max_size=63G
# dbms.tx_state.memory_allocation=ON_HEAP
# dbms.memory.pagecache.size=512M
# dbms.connector.http.listen_address=:$docker2_port1
# dbms.connector.bolt.listen_address=:$docker2_port2
# dbms.default_listen_address=0.0.0.0
# dbms.security.auth_enabled=true
# cypher.lenient_create_relationship = true
# dbms.default_database=$database
# dbms.directories.logs=/logs
# EOL"

#     docker restart $docker1
#     docker restart $docker2

    echo "Run CypherRewrite for $path"
    cd ./CypherRewrite/build || exit 3
    ./CypherRewrite $path
    cd ../..
    echo "complete rewrite"

    echo "start init for $dataset"
    if [ "$is_delete" = true ]; then
         echo python create_views.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2 --is_delete
         python create_views.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2 --is_delete
    else
         echo python create_views.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2
         python create_views.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2
    fi
else 
    echo "start run new_run.py for $dataset (test_type=$test_type)"
    python new_run.py -path $path -tugraph $tugraph -tuurl $tugraph_url -tuuser $tugraph_user -tupwd $tugraph_password -neurl1 $neo4j_url1 -neuser1 $neo4j_user1 -nepwd1 $neo4j_password1 -neurl2 $neo4j_url2 -neuser2 $neo4j_user2 -nepwd2 $neo4j_password2 -tt $test_type --profile $is_profile --cycle $cycle
    echo "complete run for $dataset"
fi