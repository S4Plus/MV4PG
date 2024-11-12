SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

python3 view_test_opt.py -g finbenchSf10 -f finbench -pr True --cycle 1
python3 view_test_opt.py -g ldbcSf1 -f ldbcSf1 -pr True --cycle 1