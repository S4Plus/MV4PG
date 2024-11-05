from neo4j import GraphDatabase

class Neo4jConnector:
    def __init__(self, url, user, password):
        self.driver = GraphDatabase.driver(url, auth=(user, password))

    def close(self):
        self.driver.close()

    def run_cypher(self, query, parameters=None):
        with self.driver.session() as session:
            result = session.run(query, parameters)
            # 提取所有记录到列表
            records = [record for record in result]
            # 获取查询的 summary
            
            summary = result.consume()
            return records, summary

if __name__ == "__main__":
    url = "bolt://localhost:7690"
    user = "neo4j"
    password = "123456"

    connector = Neo4jConnector(url, user, password)
    
    # 执行查询
    result1, summary = connector.run_cypher("profile match(n:Account) return n limit 10")
    print(summary.profile)
    # 打印查询的结果
    print(result1)