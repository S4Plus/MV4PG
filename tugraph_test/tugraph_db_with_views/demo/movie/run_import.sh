#!/bin/bash
mkdir -p /var/lib/lgraph/
/tugraph-db_graph_views/build/output/lgraph_import --dir /data --verbose 2 -c import.json -g MovieDemo1  --continue_on_error 1 --overwrite 1 --online false
rm -rf import_tmp

/tugraph-db_graph_views/build/output/lgraph_import --dir /data/sf1 --verbose 2 -c import.conf --continue_on_error 1 --overwrite 1 --online false -g ldbcSf1

echo "IMPORT DONE."