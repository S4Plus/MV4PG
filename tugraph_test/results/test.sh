if [ "$#" -gt 0 ]; then
  is_delete=$1
else
  is_delete=False
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"
./ldbcSf1/test_snb.sh $is_delete
cd "$SCRIPT_DIR"
./finbench/test_finbench.sh $is_delete
./profile.sh
./multi_delete.sh