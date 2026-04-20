SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/.."

if [ "$#" -gt 0 ]; then
  cycle=$1
else
  cycle=5
fi

if [ "$#" -gt 1 ]; then
  pr=$2
else
  pr=False
fi

python3 ./multi_delete.py -g chem -f chem --create_num 1 --cycle $cycle --pr $pr
python3 ./multi_delete.py -g chem -f chem --create_num 10 --cycle $cycle --pr $pr
python3 ./multi_delete.py -g chem -f chem --create_num 100 --cycle $cycle --pr $pr
python3 ./multi_delete.py -g chem -f chem --create_num 1000 --cycle $cycle --pr $pr
python3 ./multi_delete.py -g chem -f chem --create_num 10000 --cycle $cycle --pr $pr