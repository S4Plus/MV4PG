#!/bin/bash
# Usage: total.sh [need_init] [test_type]
# need_init: true/false (default false)
# test_type: 0=all,1=Read,2=Write,3=multi_delete

need_init=${1:-true}
test_type=${2:-0}

scripts_dir=$(dirname "$0")

echo "Running total tests: need_init=$need_init, test_type=$test_type"

datasets=(chem ldbcSf1 ldbcSf10)
first=true
for ds in "${datasets[@]}"; do
    if [ "$first" = true ] && [ "$need_init" = true ]; then
        echo "Running $ds with init"
        "$scripts_dir/run_dataset.sh" $ds true $test_type
        first=false
    else
        echo "Running $ds without init"
        "$scripts_dir/run_dataset.sh" $ds false $test_type
    fi
done

echo "All datasets tested."
