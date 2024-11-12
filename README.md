# Overview
The experimental test results are available in `results.xlsx`. For a detailed explanation, please refer to `Evaluation Result`. The testing processes for TuGraph and Neo4j are documented in `TuGraph_Test` and `Neo4j_Test`, respectively.
# TuGraph Test
## Docker
First, pull the TuGraph image and create the container, then copy this repository to the container.
```
docker pull tugraph/tugraph-compile-centos7
docker run -d --name tugraph -p 7071:7071 -p 7072:7072 -it tugraph/tugraph-compile-centos7 bash
docker cp ../MV4PG tugraph:/
```
## Test
Then execute the following script in the docker container.
```
./tugraph_test.sh
```

# Neo4j Test


# Evaluation Result
`results.xlsx` contains 7 tables: `tugraph_snb`, `tugraph_finbench`, `neo4j_snb`, `neo4j_finbench`, `delete_performance`, `snbSf1`, and `finbenchSf10`. The tables `tugraph_snb` and `tugraph_finbench` are the test results of the two datasets in TuGraph, including various data used in the paper, such as time, speedup ratios, profile results, etc. The tables `neo4j_snb` and `neo4j_finbench` are the test results in Neo4j, similar to the previous two tables. `delete_performance` is the statistical data measuring the effects of deleting different numbers of edges. `snbSf1` and `finbenchSf10` are statistical data of the two datasets.