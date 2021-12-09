#!/bin/bash
set -e

ROOT="$PWD"
CNF_PATH="$ROOT/resources/cnf/pancake_vs_selection_7_4.cnf"
CFG_PATH="$ROOT/resources/config/base.json"
SOLVE_BIN="$ROOT/bazel-bin/evol/main"
TEST_BIN="$ROOT/bazel-bin/evol/test"
VERBOSE="2"
DO_ALL=""

function print_help() {
    echo "Usage: ./run.sh [options]"
    echo "  Options:"
    echo "   -h|--help           Display this message"
    echo "   -d|--backdoor       Enable backdoor"
    echo "   -b|--build          Do build"
    echo "   -f|--format         Do reformat"
    echo "   -p|--proto          Do proto rebuild"
    echo "   -t|--test Do test"
    echo "   -v|--verbose <x>    Set verbose level to x[=2]"
    echo "   -c|--config <path>  Path to config"
    echo "   -i|--input <path>   Path to CNF"
    echo "   -s|--solve          Do solve"
    echo "   -a|--all            Do all (equal to -bfpst)"
    echo "   --sync              Synchronize external dependencies."
    exit 1
}

function parse_option() {
    local opt="$1"
    for (( i = 1; i < ${#opt}; ++i )); do
        case "${opt:$i:1}" in
            h) {
                print_help
            } ;;
            d) {
                BACKDOOR="--backdoor"
            } ;;
            f) {
                FORMAT="YES"
            } ;;
            b) {
                BUILD="YES"
            } ;;
            p) {
                PROTO="YES"
            } ;;
            s) {
                SOLVE="YES"
            } ;;
            t) {
                TEST="YES"
            } ;;
            a) {
                DO_ALL="YES"
            } ;;
            *) {
                echo "Unkown option: -${opt:$i:1}"
                exit 1
            } ;;
        esac
    done
}

function parse_options() {
    while ! [[ -z "$1" ]]; do
        case "$1" in
            -v|--verbose) {
                shift
                VERBOSE="$1"
            } ;;
            -i|--input) {
                shift
                CNF_PATH="$1"
            } ;;
            -c|--config) {
                shift
                CFG_PATH="$1"
            } ;;
            --help) {
                print_help
            } ;;
            --backdoor) {
                BACKDOOR="--backdoor"
            } ;;
            --format) {
                FORMAT="YES"
            } ;;
            --build) {
                BUILD="YES"
            } ;;
            --proto) {
                PROTO="YES"
            } ;;
            --solve) {
                SOLVE="YES"
            } ;;
            --test) {
                TEST="YES"
            } ;;
            --all) {
                DO_ALL="YES"
            } ;;
            --filter) {
                shift
                FILTER="--gtest_filter=$1"
            } ;;
            --sync) {
                SYNC="YES"
            } ;;
            -*) {
                parse_option $1
            } ;;
            *) {
                echo "Unkown option: $1"
                exit 1
            } ;;
        esac

        shift
    done

    ARGS=""

    FORMAT="$FORMAT$DO_ALL"
    BUILD="$BUILD$DO_ALL"
    PROTO="$PROTO$DO_ALL"
    SOLVE="$SOLVE$DO_ALL"
    TEST="$TEST$DO_ALL"
}


function main() {
    parse_options "$@"

    if ! [[ -z "$SYNC" ]]; then
        cd "$ROOT/ext/libpqxx"
        git checkout '7.6.0'
        ./configure
        sudo make install
        cd "$ROOT"
    fi

    if ! [[ -z "$FORMAT" ]]; then
        find . -iname *.h -o -iname *.hpp -o -iname *.cpp -o -iname *.cc | xargs clang-format -i
    fi

    if ! [[ -z "$PROTO" ]]; then
        rm ./evol/proto/config.pb.h || true
        rm ./evol/proto/config.pb.cc || true
        bazel build '//evol/proto:*' $ARGS
        ln -s `pwd`/bazel-bin/evol/proto/config.pb.h ./evol/proto/config.pb.h
        ln -s `pwd`/bazel-bin/evol/proto/config.pb.cc ./evol/proto/config.pb.cc
    fi

    if ! [[ -z "$BUILD" ]]; then
        bazel build //evol:main //evol:test $ARGS
    fi

    if ! [[ -z "$TEST" ]]; then
        GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $TEST_BIN $FILTER
    fi

    if ! [[ -z "$SOLVE" ]]; then
        GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $SOLVE_BIN \
            $BACKDOOR \
            --input "$CNF_PATH" \
            --config "$CFG_PATH" \
            $ARGS
    fi
}

main "$@"
