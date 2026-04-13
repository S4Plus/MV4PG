SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

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

./ldbcSf1_new/multi_delete_snb.sh ${cycle} ${pr}
./ldbcSf10_new/multi_delete_snb.sh ${cycle} ${pr}
./chem/multi_delete_chem.sh ${cycle} ${pr}