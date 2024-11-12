SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

./tugraph_test/tugraph_prepare.sh
./tugraph_test/evaluation/test.sh