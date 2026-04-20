#!/usr/bin/env python3
"""Run a Cypher statement against Neo4j and print results as JSON.

Usage:
  test_cypher.py --cypher "MATCH (n) RETURN n LIMIT 5" [--url bolt://localhost:7687] [--user neo4j] [--password pwd]
  or: echo "MATCH ..." | test_cypher.py
"""
import argparse
import json
import sys
from neo4j import GraphDatabase


def parse_args():
    p = argparse.ArgumentParser(description="Run Cypher on Neo4j and return JSON results")
    p.add_argument('--url', default='bolt://localhost:7687')
    p.add_argument('--user', default='neo4j')
    p.add_argument('--password', default='12345678')
    p.add_argument('--cypher', help='Cypher query string. If omitted, read from stdin')
    return p.parse_args()


def record_to_obj(record):
    # Convert neo4j.Record to JSON-serializable dict
    out = {}
    for key in record.keys():
        val = record.get(key)
        try:
            json.dumps(val)
            out[key] = val
        except TypeError:
            # Try to convert nodes/relationships/maps to basic structures
            out[key] = convert_value(val)
    return out


def convert_value(v):
    # Handle common neo4j types (Node, Relationship, list, dict)
    try:
        from neo4j.graph import Node, Relationship
    except Exception:
        Node = Relationship = None

    if v is None:
        return None
    if isinstance(v, (str, int, float, bool)):
        return v
    if isinstance(v, list):
        return [convert_value(x) for x in v]
    if isinstance(v, dict):
        return {k: convert_value(val) for k, val in v.items()}
    if Node is not None and isinstance(v, Node):
        return {"id": v.id, "labels": list(v.labels), "properties": dict(v.items())}
    if Relationship is not None and isinstance(v, Relationship):
        return {"id": v.id, "type": v.type, "start": v.start_node.id if hasattr(v, 'start_node') else None,
                "end": v.end_node.id if hasattr(v, 'end_node') else None, "properties": dict(v.items())}
    # Fallback to string
    return str(v)


def main():
    args = parse_args()

    if args.cypher:
        cypher = args.cypher
    else:
        # read full stdin
        cypher = sys.stdin.read().strip()

    if not cypher:
        print('No Cypher provided', file=sys.stderr)
        sys.exit(2)

    driver = GraphDatabase.driver(args.url, auth=(args.user, args.password))
    try:
        with driver.session() as session:
            result = session.run(cypher)
            records = [record_to_obj(rec) for rec in result]
            # Try to get summary counters
            summary = result.consume()
            out = {"records": records}
            try:
                out["counters"] = {
                    "nodes_created": summary.counters.nodes_created,
                    "nodes_deleted": summary.counters.nodes_deleted,
                    "relationships_created": summary.counters.relationships_created,
                    "relationships_deleted": summary.counters.relationships_deleted,
                }
            except Exception:
                pass
            print(json.dumps(out, indent=2, ensure_ascii=False))
    finally:
        driver.close()


if __name__ == '__main__':
    main()
