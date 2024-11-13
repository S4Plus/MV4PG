# docker pull docker.1ms.run/neo4j:4.4.2
# wget http://home.ustc.edu.cn/~angels/import_snb_sf1.zip
# wget http://home.ustc.edu.cn/~angels/import_finbench_sf10.zip
# Create directories for neo4j1
mkdir -p ./neo4j1/data
mkdir -p ./neo4j1/logs
mkdir -p ./neo4j1/conf
mkdir -p ./neo4j1/import
mkdir -p ./neo4j1/plugins

# Create directories for neo4j2  
mkdir -p ./neo4j2/data
mkdir -p ./neo4j2/logs
mkdir -p ./neo4j2/conf
mkdir -p ./neo4j2/import
mkdir -p ./neo4j2/plugins

# Start neo4j1 container
docker run -d \
    --name neo4j1 \
    -v $PWD/neo4j1/data:/var/lib/neo4j/data \
    -v $PWD/neo4j1/logs:/var/lib/neo4j/logs \
    -v $PWD/neo4j1/conf:/var/lib/neo4j/conf \
    -v $PWD/neo4j1/import:/var/lib/neo4j/import \
    -v $PWD/neo4j1/plugins:/var/lib/neo4j/plugins \
    -e NEO4J_AUTH=neo4j/123456 \
    -p 7443:7443 \
    -p 7690:7690 \
    docker.1ms.run/neo4j:4.4.2

# Start neo4j2 container
docker run -d \
    --name neo4j2 \
    -v $PWD/neo4j2/data:/var/lib/neo4j/data \
    -v $PWD/neo4j2/logs:/var/lib/neo4j/logs \
    -v $PWD/neo4j2/conf:/var/lib/neo4j/conf \
    -v $PWD/neo4j2/import:/var/lib/neo4j/import \
    -v $PWD/neo4j2/plugins:/var/lib/neo4j/plugins \
    -e NEO4J_AUTH=neo4j/352541141 \
    -p 7444:7444 \
    -p 7691:7691 \
    docker.1ms.run/neo4j:4.4.2
# Install unzip in neo4j1
docker exec neo4j1 apt-get update
docker exec neo4j1 apt-get install -y unzip

# # Install unzip in neo4j2
docker exec neo4j2 apt-get update
docker exec neo4j2 apt-get install -y unzip

# Wait for containers to be ready
# sleep 30
# Unzip finbench data
docker cp import_finbench_sf10.zip neo4j1:/var/lib/neo4j/import/
docker cp import_finbench_sf10.zip neo4j2:/var/lib/neo4j/import/
docker cp import_snb_sf1.zip neo4j1:/var/lib/neo4j/import/
docker cp import_snb_sf1.zip neo4j2:/var/lib/neo4j/import/
# Run import script in neo4j1
docker exec neo4j1 bash -c "cd /var/lib/neo4j/import && unzip import_finbench_sf10.zip && unzip import_snb_sf1.zip"
# Run import script in neo4j2  
docker exec neo4j2 bash -c "cd /var/lib/neo4j/import && unzip import_finbench_sf10.zip && unzip import_snb_sf1.zip"

# # Configure neo4j1
# docker exec neo4j1 bash -c "cat > /var/lib/neo4j/conf/neo4j.conf << EOL
# dbms.tx_log.rotation.retention_policy=100M size
# dbms.memory.heap.initial_size=63G
# dbms.memory.heap.max_size=63G
# dbms.tx_state.memory_allocation=ON_HEAP
# dbms.memory.pagecache.size=512M
# dbms.connector.http.listen_address=:7443
# dbms.connector.bolt.listen_address=:7690
# dbms.default_listen_address=0.0.0.0
# dbms.security.auth_enabled=true
# cypher.lenient_create_relationship = true
# dbms.default_database=finbenchSf10
# dbms.directories.logs=/logs
# EOL"

# # Configure neo4j2
# docker exec neo4j2 bash -c "cat > /var/lib/neo4j/conf/neo4j.conf << EOL
# dbms.tx_log.rotation.retention_policy=100M size
# dbms.memory.heap.initial_size=63G
# dbms.memory.heap.max_size=63G
# dbms.tx_state.memory_allocation=ON_HEAP
# dbms.memory.pagecache.size=512M
# dbms.connector.http.listen_address=:7444
# dbms.connector.bolt.listen_address=:7691
# dbms.default_listen_address=0.0.0.0
# dbms.security.auth_enabled=true
# cypher.lenient_create_relationship = true
# dbms.default_database=finbenchSf10
# dbms.directories.logs=/logs
# EOL"

docker exec neo4j1 bash -c "cd /var/lib/neo4j/import/import_finbench_sf10 && ./import.sh"
docker exec neo4j2 bash -c "cd /var/lib/neo4j/import/import_finbench_sf10 && ./import.sh"
docker exec neo4j1 bash -c "cd /var/lib/neo4j/import/import_snb_sf1 && ./import.sh"
docker exec neo4j2 bash -c "cd /var/lib/neo4j/import/import_snb_sf1 && ./import.sh"
# Restart containers to apply new configuration
docker restart neo4j1
docker restart neo4j2

# Wait for containers to restart
# sleep 30
