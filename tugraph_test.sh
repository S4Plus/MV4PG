SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

chmod +x ./tugraph_test/tugraph_prepare.sh
chmod +x ./tugraph_test/view_test/test.sh
./tugraph_test/tugraph_prepare.sh
./tugraph_test/view_test/test.sh