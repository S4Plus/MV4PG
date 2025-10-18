# docker pull neo4j:4.4.2
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
    -e NEO4J_dbms_connector_bolt_advertised__address=:7687 \
    -e NEO4J_dbms_connector_http_advertised__address=:7474 \
    -e NEO4J_dbms_connector_https_advertised__address=:7474 \
    --network=host \
    neo4j:4.4.2 

# Start neo4j2 container
docker run -d \
    --name neo4j2 \
    -v $PWD/neo4j2/data:/var/lib/neo4j/data \
    -v $PWD/neo4j2/logs:/var/lib/neo4j/logs \
    -v $PWD/neo4j2/conf:/var/lib/neo4j/conf \
    -v $PWD/neo4j2/import:/var/lib/neo4j/import \
    -v $PWD/neo4j2/plugins:/var/lib/neo4j/plugins \
    -e NEO4J_AUTH=neo4j/352541141 \
    -e NEO4J_dbms_connector_bolt_advertised__address=:7688 \
    -e NEO4J_dbms_connector_http_advertised__address=:7475 \
    -e NEO4J_dbms_connector_https_advertised__address=:7475 \
    -e NEO4J_dbms_connector_bolt_listen__address=:7688 \
    -e NEO4J_dbms_connector_http_listen__address=:7475 \
    -e NEO4J_dbms_connector_https_listen__address=:7475 \
    --network=host \
    neo4j:4.4.2 
# Install unzip in neo4j1
docker exec neo4j1 apt-get update
docker exec neo4j1 apt-get install -y unzip

# # Install unzip in neo4j2
docker exec neo4j2 apt-get update
docker exec neo4j2 apt-get install -y unzip

# Wait for containers to be ready
# sleep 30

docker cp import_data_sf10.zip neo4j1:/var/lib/neo4j/import/
docker cp import_data_sf10.zip neo4j2:/var/lib/neo4j/import/
docker cp import_snb_sf1.zip neo4j1:/var/lib/neo4j/import/
docker cp import_snb_sf1.zip neo4j2:/var/lib/neo4j/import/
# Run import script in neo4j1
docker exec neo4j1 bash -c "cd /var/lib/neo4j/import && unzip import_data_sf10.zip && unzip import_snb_sf1.zip"
# Run import script in neo4j2  
docker exec neo4j2 bash -c "cd /var/lib/neo4j/import && unzip import_data_sf10.zip && unzip import_snb_sf1.zip"


docker exec neo4j1 bash -c "cd /var/lib/neo4j/import/import_data_sf10 && ./import.sh"
docker exec neo4j2 bash -c "cd /var/lib/neo4j/import/import_data_sf10 && ./import.sh"
docker exec neo4j1 bash -c "cd /var/lib/neo4j/import/import_snb_sf1 && ./import.sh"
docker exec neo4j2 bash -c "cd /var/lib/neo4j/import/import_snb_sf1 && ./import.sh"
# Restart containers to apply new configuration
docker restart neo4j1
docker restart neo4j2

# Wait for containers to restart
# sleep 30
