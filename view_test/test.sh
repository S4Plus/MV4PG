if [ "$#" -gt 0 ]; then
  is_delete=$1
else
  is_delete=False
fi


# /tugraph-db-without_opt/build/output/lgraph_server -c /tugraph-db-without_opt/demo/movie/lgraph.json
# /tugraph-db_graph_views/build/output/lgraph_server -c /tugraph-db_graph_views/demo/movie/lgraph.json
python3 view_test/view_test_create.py --is_delete $is_delete
python3 view_test/view_test_maintenance.py
python3 view_test/view_test_opt.py