#!/bin/bash
source scripts/base.sh

ROOT="$PWD"
VERBOSE="2"
RESOURCES_DIR="$ROOT/resources"
CNF_PATH="$ROOT/resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf"
CFG_ROOT="$ROOT/resources/config"
LOG_CFG_PATH="$ROOT/resources/config/log.json"
SOLVE_BIN="$ROOT/bazel-bin/cli/solve"
TEST_BIN="$ROOT/bazel-bin/core/test"
SLV_CFG="naive.json"
INFRA_BIN="$ROOT/bazel-bin/cli/infra"
PSQL_HOST="51.250.2.131"

NEXT_NATIVE=0
BUILD_DEBUG=""
RUN_CMD=""
BUILD_CFG="dev_fast"

function do_doc() {
    rm -rf doc/* || true
    PROJECT_NAME="itmo-parsat" doxygen ./resources/doxygen.conf &> /dev/null
}

function do_input() {
    CNF_PATH="$1"
}

function do_config() {
    SLV_CFG="$1.json"
}

function do_set_build_mode() {
    BUILD_CFG="$1"
}

function do_run_gdb() {
    RUN_CMD+="gdb --args"
}

function do_run_perf() {
    RUN_CMD+="perf record --call-graph dwarf"
}

function do_run_valgrind() {
    RUN_CMD+="valgrind --leak-check=full"
}

function do_format() {
    find . -iname *.h -o -iname *.hpp -o -iname *.cpp -o -iname *.cc | xargs clang-format -i
}

function do_build_solve() {
    bazel build //cli:solve --config=$BUILD_CFG $@
}

function do_build_unit() {
    bazel build //core:test --config=$BUILD_CFG $@
}

function do_build_infra() {
    bazel build //cli:infra --config=$BUILD_CFG $@
}

function do_build_proto() {
    rm -rf ./core/proto/*.pb.*
    bazel build //core/proto:* --config=$BUILD_CFG $@
    ln -s `pwd`/bazel-bin/core/proto/solve_config.pb.h ./core/proto/solve_config.pb.h
    ln -s `pwd`/bazel-bin/core/proto/solve_config.pb.cc ./core/proto/solve_config.pb.cc
    ln -s `pwd`/bazel-bin/core/proto/logging_config.pb.h ./core/proto/logging_config.pb.h
    ln -s `pwd`/bazel-bin/core/proto/logging_config.pb.cc ./core/proto/logging_config.pb.cc
}

function do_set_verbose() {
    VERBOSE="$1"
}

function do_unit() {
    $RUN_CMD $TEST_BIN --verbose $VERBOSE $@
}

function do_infra() {
    $RUN_CMD $INFRA_BIN \
        --branch $(git status | grep branch | awk '{print $3}') \
        --commit $(git rev-parse --verify HEAD) \
        --resources-dir $RESOURCES_DIR \
        --working-dir $INFRA_DIR \
        --exec $SOLVE_BIN \
        --pg-host $PSQL_HOST \
        $@
}

function do_solve() {
    if [[ -z "$@" ]]; then
        $RUN_CMD $SOLVE_BIN \
            --verbose "$VERBOSE" \
            --input "$CNF_PATH" \
            --config "$CFG_ROOT/$SLV_CFG" \
            --log-config "$LOG_CFG_PATH"
    else
        $RUN_CMD $SOLVE_BIN $@
    fi
}

function do_psql() {
    psql -d infra_db -U ips -h "$PSQL_HOST"
}

function do_set_pg_host() {
    PSQL_HOST="$1"
}

function do_desc() {
    echo "Running and/or building IPS."
    echo "Usage: ./run.sh option* -- native-option*"
}

add_option "-g|--build-cfg" "Set build mode"             do_set_build_mode 1
add_option "-b|--build" "    Build cli binary"           do_build_solve    0
add_option "--build-unit" "  Build unit tests"           do_build_unit     0
add_option "--build-infra" " Build integration tests"    do_build_infra    0
add_option "--build-proto" " Rebuild and s-link proto"   do_build_proto    0
add_option "--build-doc" "   Build documentation"        do_doc            0
add_option "--run-debug" "   Run with gdb"               do_run_gdb        0
add_option "--run-perf" "    Run with perf"              do_run_perf       0
add_option "--run-valgrind" "Run with valgrind"          do_run_valgrind   0
add_option "-i|--input" "    Input CNF path"             do_input          1
add_option "-c|--config" "   Specify config"             do_config         1
add_option "-f|--format" "   Apply clang-format"         do_format         0
add_option "-v|--verbose" "  Set verbosity level [=2]"   do_set_verbose    1
add_option "-u|--unit" "     Run unit tests"             do_unit           0
add_option "--run-infra" "   Run integration tests"      do_infra          0
add_option "--psql" "        Connect to DB"              do_psql           0
add_option "--psql-host" "   Set DB host"                do_set_pg_host    1
add_option "-s|--solve" "    Run cli binary"             do_solve          0

function main() {
    parse_options "$@"
}

_main "$@"
