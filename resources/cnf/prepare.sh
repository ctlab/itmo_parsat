#!/bin/bash
set -eu

is_sat() {
    cwd=$(pwd)
    cd ../../
    ./run.sh -c naive -i "./resources/cnf/$1" -s | grep UNSAT
    result=$?
    cd $cwd
    return $result
}

for path in $(find . -name '*.cnf' -type f); do
    cnf=$(basename $path)

    if ! [[ $cnf == unsat* ]] && ! [[ $cnf == sat* ]]; then
        is_sat "$path" && { echo "$cnf is UNSAT"; pref="unsat_"; } || { echo "$cnf is SAT"; pref="sat_"; }
        name="${cnf%.*}"
        base_path=$(dirname $path)
        mv "$path" "$base_path/$pref$name@0.cnf"
    fi
done
