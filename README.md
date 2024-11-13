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
## Prerequisites

## Prerequisites

- Python 3.x
- Docker
- TuGraph Server
- Required Python packages:
  - neo4j
  - TuGraphClient
  - argparse

## Installation

1. Clone the repository:
```bash
git clone [repository-url]
```

2. Install required dependencies:
```bash
pip install neo4j TuGraphClient argparse
```

3. Set up Neo4j instances:
   - run the shell script db.sh in the root directory of the project.

4. Set up TuGraph:
   - Install and configure TuGraph server
   - Create necessary graphs and permissions
5. set the path in shell script (ldbc_test.sh, finbench_test.sh) as the path of the test file in your local machine . If you need to initialize the databases,please add the parameters <true> after the shell script.
6. run the shell script 
## Configuration

### Default Configuration
The tool uses the following default configuration:

```python
neo4j_url1 = "bolt://localhost:7690"
neo4j_user1 = "neo4j"
neo4j_password1 = "123456"
neo4j_url2 = "bolt://localhost:7691"
neo4j_user2 = "neo4j"
neo4j_password2 = "352541141"
tugraph_url = '127.0.0.1:7073'
tugraph_user = 'admin'
tugraph_password = '73@TuGraph'
tugraph_graph = 'finbenchSf10'
```
You can change the values to make sure the project can run correctly in your local machine 
### Command Line Arguments

Configure the tool using these command-line arguments:

```bash
python new_run.py [-h] [-path PATH] [-tugraph TUGRAPH] [-tuurl TUURL] 
                  [-tupwd TUPWD] [-tuuser TUUSER] [-neurl1 NEURL1] 
                  [-nepwd1 NEPWD1] [-neuser1 NEUSER1] [-neurl2 NEURL2] 
                  [-nepwd2 NEPWD2] [-neuser2 NEUSER2]
```

Arguments:
- `-path`: Directory path containing test files
- `-tugraph`: TuGraph graph name
- `-tuurl`: TuGraph server URL
- `-tupwd`: TuGraph password
- `-tuuser`: TuGraph username
- `-neurl1`: First Neo4j instance URL
- `-nepwd1`: First Neo4j instance password
- `-neuser1`: First Neo4j instance username
- `-neurl2`: Second Neo4j instance URL
- `-nepwd2`: Second Neo4j instance password
- `-neuser2`: Second Neo4j instance username

## Directory Structure

Required directory structure:
```
path/
├── ReadQueries/
│   └── all.txt          # Read query test cases
├── WriteQueries/
│   ├── all.txt          # Write query test cases
│   └── create_edge.cypher
├── recover/
│   ├── all.txt          # Recovery test cases
│   └── recover_ce.cypher
├── result/              # Test results directory
│   ├── read/
│   ├── write/
│   └── recovery/
└── output.json          # View definitions and triggers
```

# Evaluation Result
`results.xlsx` contains 7 tables: `tugraph_snb`, `tugraph_finbench`, `neo4j_snb`, `neo4j_finbench`, `delete_performance`, `snbSf1`, and `finbenchSf10`. The tables `tugraph_snb` and `tugraph_finbench` are the test results of the two datasets in TuGraph, including various data used in the paper, such as time, speedup ratios, profile results, etc. The tables `neo4j_snb` and `neo4j_finbench` are the test results in Neo4j, similar to the previous two tables. `delete_performance` is the statistical data measuring the effects of deleting different numbers of edges. `snbSf1` and `finbenchSf10` are statistical data of the two datasets.