# Neo4j View Maintenance Testing Tool

This tool is designed to test and compare the performance of view maintenance operations in Neo4j, with and without optimization. It supports testing both read and write operations, and includes functionality to test with TuGraph optimization.

So Testing on Neo4j relies on TuGraph, so make sure you have the TuGraph service started!
## Installation
1. Install required dependencies:
```bash
pip install neo4j TuGraphClient
```

2. Set up Neo4j instances:
   - Configure two separate Neo4j instances on different ports
   - Ensure both instances are running and accessible

3. set the path in shell script (ldbc_test.sh, finbench_test.sh) as the path of the test file in your local machine . If you need to initialize the databases,please add the parameters <true> after the shell script.
4. run the shell script 
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