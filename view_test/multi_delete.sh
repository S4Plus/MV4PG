SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"
./finbench/multi_delete_finbench.sh
./ldbcSf1/multi_delete_snb.sh