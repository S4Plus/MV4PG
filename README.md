# Overview
The experimental test results are in `results.xlsx`. The test process in TuGraph refers to `TuGraph_Test`, and the test process in Neo4j refers to `Neo4j_Test`.
# TuGraph Test
## Docker
First, pull the TuGraph image and create the container, then copy this repository to the container
```
docker pull tugraph/tugraph-compile-centos7
docker run -d -p 7071:7071 7072:7072 -it tugraph/tugraph-compile-centos7 tugraph
docker cp ../MV4PG tugraph:/
```
## Test
Then execute the following script in the docker container
```
./tugraph_test.sh
```

# Neo4j Test

