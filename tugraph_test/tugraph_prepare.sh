SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# compile
cd "$SCRIPT_DIR/tugraph_db_without_views"
mkdir build && cd build
cmake ..
make -j32

cd "$SCRIPT_DIR/tugraph_db_with_views"
mkdir build && cd build
cmake ..
make -j32

# import data
cd "$SCRIPT_DIR"
wget home.ustc.edu.cn/~angels/import_snb_sf1.zip # snbSf1
wget home.ustc.edu.cn/~angels/import_finbench_sf10.zip # finbenchSf10
unzip import_snb_sf1.zip 
unzip import_finbench_sf10.zip

cd "$SCRIPT_DIR/import_snb_sf1"
../tugraph_db_without_views/build/output/lgraph_import --dir /data_wo_opt --verbose 2 -c import.conf -g ldbcSf1  --continue_on_error 1 --overwrite 1 --online false
../tugraph_db_with_views/build/output/lgraph_import --dir /data_opt --verbose 2 -c import.conf -g ldbcSf1  --continue_on_error 1 --overwrite 1 --online false

cd "$SCRIPT_DIR/import_finbench_sf10"
../tugraph_db_without_views/build/output/lgraph_import --dir /data_wo_opt --verbose 2 -c import.conf -g finbenchSf10  --continue_on_error 1 --overwrite 1 --online false --delimiter "|"
../tugraph_db_with_views/build/output/lgraph_import --dir /data_opt --verbose 2 -c import.conf -g finbenchSf10  --continue_on_error 1 --overwrite 1 --online false --delimiter "|"

# start TuGraph
cd "$SCRIPT_DIR/tugraph_db_without_views"
./build/output/lgraph_server -d start --directory /data_wo_opt --port 7071 --rpc_port 9091
cd "$SCRIPT_DIR/tugraph_db_with_views"
./build/output/lgraph_server -d start --directory /data_opt --port 7072 --rpc_port 9092

# python
python3 -m pip install TuGraphClient

echo "prepare done"