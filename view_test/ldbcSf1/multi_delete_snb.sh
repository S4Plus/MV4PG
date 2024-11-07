SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "../$SCRIPT_DIR"

if [ "$#" -gt 0 ]; then
  cycle=$1
else
  cycle=5
fi

python3 ./multi_delete.py -g ldbcSf1 --create_num 1 --cycle $cycle
python3 ./multi_delete.py -g ldbcSf1 --create_num 10 --cycle $cycle
python3 ./multi_delete.py -g ldbcSf1 --create_num 100 --cycle $cycle
python3 ./multi_delete.py -g ldbcSf1 --create_num 1000 --cycle $cycle
python3 ./multi_delete.py -g ldbcSf1 --create_num 10000 --cycle $cycle