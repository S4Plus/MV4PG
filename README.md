# TuGraph Test
## Docker
First pull the TuGraph image and create the container, copy this repository to the container
```
docker pull tugraph/tugraph-compile-centos7
docker run -d -p 7071:7071 7072:7072 -it tugraph/tugraph-compile-centos7 tugraph
docker cp ../tugraph-db tugraph:/
```
## Test
Then execute the following script in docker
```
./tugraph_test.sh
```

# Neo4j Test



# Evaluation Result
The results of the experiment are in `results.xlsx`