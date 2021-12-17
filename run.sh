#!/bin/bash
set -e

ROOT="$PWD"
VERBOSE="2"
DO_ALL=""
RESOURCES_DIR="./resources"

CNF_PATH="$ROOT/resources/cnf/unsat_pancake_vs_selection_7_4.cnf"
SLV_CFG_PATH="$ROOT/resources/config/algorithm.json"
LOG_CFG_PATH="$ROOT/resources/config/log.json"
SOLVE_BIN="$ROOT/bazel-bin/cli/solve"
TEST_BIN="$ROOT/bazel-bin/core/test"

INFRA_BIN="$ROOT/bazel-bin/cli/infra"
INFRA_DIR="./artifacts-infra/"
INFRA_DB_NAME="infra_db"
INFRA_DB_SETUP="./infra/resources/create_tables.sql"

NEXT_NATIVE=0
BUILD_DEBUG=""
RUN_GDB=""

declare -a order
declare -A actions
declare -A descs
declare -A needs_arg

function add_option() {
    PATTERN="$1"
    DESC="$2"
    ACTION="$3"
    NEEDS_ARG="$4"
    order+=($PATTERN)
    actions["$PATTERN"]="$ACTION"
    descs["$PATTERN"]="$DESC"
    needs_arg["$PATTERN"]="$NEEDS_ARG"
}

function do_help() {
    echo "Usage: ./run.sh option* -- native-option*"
    for i in "${!order[@]}"; do
        pattern="${order[$i]}"
        echo "    $pattern        ${descs[$pattern]}"
    done
    exit 1
}

function do_backdoor() {
    BACKDOOR="--backdoor"
}

function do_build_debug() {
    BUILD_DEBUG="--compilation_mode=dbg"
}

function do_run_gdb() {
    RUN_GDB="gdb --args"
}

function do_format() {
    find . -iname *.h -o -iname *.hpp -o -iname *.cpp -o -iname *.cc | xargs clang-format -i
}

function do_build_solve() {
    bazel build //cli:solve $BUILD_DEBUG $@
}

function do_build_unit() {
    bazel build //core:test $BUILD_DEBUG $@
}

function do_build_infra() {
    bazel build //cli:infra $BUILD_DEBUG $@
}

function do_build_proto() {
    rm -rf ./core/proto/*.pb.*
    bazel build //core/proto:* $BUILD_DEBUG $@

    ln -s `pwd`/bazel-bin/core/proto/solve_config.pb.h ./core/proto/solve_config.pb.h
    ln -s `pwd`/bazel-bin/core/proto/solve_config.pb.cc ./core/proto/solve_config.pb.cc

    ln -s `pwd`/bazel-bin/core/proto/logging_config.pb.h ./core/proto/logging_config.pb.h
    ln -s `pwd`/bazel-bin/core/proto/logging_config.pb.cc ./core/proto/logging_config.pb.cc
}

function do_set_verbose() {
    VERBOSE="$1"
}

function do_unit() {
    GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $RUN_GDB $TEST_BIN $@
}

function do_infra() {
    GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $RUN_GDB $INFRA_BIN \
        --commit $(git rev-parse --verify HEAD) \
        --resources-dir $RESOURCES_DIR \
        --working-dir $INFRA_DIR \
        --exec $SOLVE_BIN \
        --dbname infra_db \
        $@
}

function do_solve() {
    if [[ -z "$@" ]]; then
        GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $RUN_GDB $SOLVE_BIN \
            $BACKDOOR \
            --input "$CNF_PATH" \
            --config "$SLV_CFG_PATH" \
            --log-config "$LOG_CFG_PATH"
    else
        GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $RUN_GDB $SOLVE_BIN \
            $BACKDOOR $@
    fi
}

function parse_options() {
    while [[ -n "$1" ]]; do
        if [[ $1 == "--" ]]; then
            shift
            echo "Next command will be executed as native"
            NEXT_NATIVE=1
        fi
        FOUND=0
        for i in "${!order[@]}"; do
            pattern="${order[$i]}"
            if [[ "$1" =~ ^$pattern$ ]]; then
                FOUND=1
                ACTION="${actions[$pattern]}"
                NEEDS_ARG="${needs_arg[$pattern]}"
                DESC="${descs[$pattern]}"
                echo "Found: '$1' ~ '$pattern' $DESC"

                shift
                if [[ $NEXT_NATIVE -eq 1 ]]; then
                    $ACTION $@
                    exit 0
                elif [[ $NEEDS_ARG -eq 1 ]]; then
                    $ACTION $1
                    shift
                else
                    $ACTION
                fi
            fi
        done
        if [[ $FOUND -eq 0 ]]; then
            echo "Invalid option: $1"
            exit 1
        fi
    done
}

add_option "-h|--help" "     Display help message"       do_help         0
add_option "-d|--backdoor" " Use backdoor search"        do_backdoor     0
add_option "-g|--debug" "    Build with debug syms"      do_build_debug  0
add_option "--run-debug" "   Run with gdb"               do_run_gdb      0
add_option "-f|--format" "   Apply clang-format"         do_format       0
add_option "-b|--build" "    Build cli binary"           do_build_solve  0
add_option "--build-unit" "  Build unit tests"           do_build_unit   0
add_option "--build-infra" " Build integration tests"    do_build_infra  0
add_option "--build-proto" " Rebuild and s-link proto"   do_build_proto  0
add_option "-v|--verbose" "  Set verbosity level [=2]"   do_set_verbose  1
add_option "-u|--unit" "     Run unit tests"             do_unit         0
add_option "--run-infra" "   Run integration tests"      do_infra        0
add_option "-s|--solve" "    Run cli binary"             do_solve        0

function main() {
    parse_options "$@"
}

main "$@"
