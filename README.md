# Overview
The experimental test results are available in `results.xlsx`. For a detailed explanation, please refer to `Evaluation Result`. The testing processes for TuGraph and Neo4j are documented in `TuGraph_Test` and `Neo4j_Test`, respectively.
# Prerequisites
- Python 3.x
- Docker
- Required Python packages:
  - neo4j
  - TuGraphClient
# TuGraph Test
If you already have a docker environment, just execute the `tugraph.sh` script directly.
```
./tugraph.sh
```
The results are stored in the `MV4PG/tugraph_test/view_test/ldbcSf1` and `MV4PG/tugraph_test/view_test/finbench` folders, respectively, within the container named `tugraph`.

# Neo4j Test

This tool is designed to test and compare the performance of view maintenance operations in Neo4j, with and without optimization. It supports testing both read and write operations, and includes functionality to test with TuGraph optimization.

## Installation
Before running the Neo4j test, ensure that the TuGraph service is started in `Tugraph Test`.

1. Install required dependencies:
```bash
pip install neo4j TuGraphClient
```

2. Set up Neo4j instances:
   - run the shell script db.sh in folder `neo4j_test`.
```bash
./db.sh
```

3. Then run the shell script in folder `neo4j_test`.
```bash
./ldbc_test.sh 
./finbench_test.sh
```
### Note
if you cannot run the shell script, please check the path of the test file and complier the CypherRewrite in the build folder.
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
tugraph_url = '127.0.0.1:7072'
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
