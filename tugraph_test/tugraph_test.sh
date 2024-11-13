SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

chmod +x ./tugraph_prepare.sh
chmod +x ./view_test/test.sh
./tugraph_prepare.sh
./view_test/test.sh