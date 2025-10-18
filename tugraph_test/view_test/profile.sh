SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

python3 view_test_opt.py -g ldbcSf1 -f ldbcSf1_new -pr True --cycle 1
python3 view_test_opt.py -g ldbcSf10 -f ldbcSf10_new -pr True --cycle 1