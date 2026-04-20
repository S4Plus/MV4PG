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
echo "anonymous now, waiting for upload..."
wget http://home.ustc.edu.cn/~angels/import_snb_sf1.zip
wget http://home.ustc.edu.cn/~angels/import_data_sf10.zip
wget http://home.ustc.edu.cn/~angels/chem.zip
unzip import_snb_sf1.zip 
unzip import_data_sf10.zip
unzip chem.zip || true

cd "$SCRIPT_DIR/import_snb_sf1"
../tugraph_db_without_views/build/output/lgraph_import --dir /data_wo_opt --verbose 2 -c import.conf -g ldbcSf1  --continue_on_error 1 --overwrite 1 --online false
../tugraph_db_with_views/build/output/lgraph_import --dir /data_opt --verbose 2 -c import.conf -g ldbcSf1  --continue_on_error 1 --overwrite 1 --online false

cd "$SCRIPT_DIR/import_data_sf10"
../tugraph_db_without_views/build/output/lgraph_import --dir /data_wo_opt --verbose 2 -c import.conf -g ldbcSf10  --continue_on_error 1 --overwrite 1 --online false
../tugraph_db_with_views/build/output/lgraph_import --dir /data_opt --verbose 2 -c import.conf -g ldbcSf10  --continue_on_error 1 --overwrite 1 --online false

cd "$SCRIPT_DIR/chem"
../tugraph_db_without_views/build/output/lgraph_import --dir /data_wo_opt --verbose 2 -c import.json -g chem  --continue_on_error 1 --overwrite 1 --online false
../tugraph_db_with_views/build/output/lgraph_import --dir /data_opt --verbose 2 -c import.json -g chem  --continue_on_error 1 --overwrite 1 --online false

# start TuGraph
cd "$SCRIPT_DIR/tugraph_db_without_views"
./build/output/lgraph_server -d start -c ./demo/movie/lgraph.json
cd "$SCRIPT_DIR/tugraph_db_with_views"
./build/output/lgraph_server -d start -c ./demo/movie/lgraph.json

echo "prepare done"