if [ "$#" -gt 0 ]; then
  is_delete=$1
else
  is_delete=False
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"
chmod +x ./ldbcSf1/test_snb.sh
./ldbcSf1/test_snb.sh $is_delete
cd "$SCRIPT_DIR"
chmod +x ./finbench/test_finbench.sh
./finbench/test_finbench.sh $is_delete

cd "$SCRIPT_DIR"
chmod +x ./profile.sh
./profile.sh

cd "$SCRIPT_DIR"
chmod +x ./multi_delete.sh
./multi_delete.sh