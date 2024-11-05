path=/home/wxd/neo4j_test/graph-views/view_maintance_2024/neo4j_Test/ldbcSf1
tugraph=ldbcSf1
need_init=$1
if [ "$need_init" = true ]; then
    python create_views.py -path $path
fi
echo "初始化完成"
python neo4j_run.py -path $path -tugraph $tugraph