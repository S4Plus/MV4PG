if [ "$#" -gt 0 ]; then
  is_delete=$1
else
  is_delete=False
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/.."

python3 ./view_test_create.py -g chem -f chem --is_delete $is_delete
python3 ./view_test_opt.py -g chem -f chem
python3 ./view_test_maintenance.py -g chem -f chem