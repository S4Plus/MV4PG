#!/bin/bash
/tugraph-db-without_opt/build/output/lgraph_import --dir /data_without_opt --verbose 2 -c import.json --continue_on_error 1 --overwrite 1 --online false -g MovieDemo1
rm -rf import_tmp

echo "IMPORT DONE."

/tugraph-db-without_opt/build/output/lgraph_import --dir /data_without_opt/sf1 --verbose 2 -c import.conf --continue_on_error 1 --overwrite 1 --online false -g ldbcSf1