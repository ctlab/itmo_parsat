#!/bin/bash
set -e

CNF_PATH="$PWD/resources/cnf/pancake_vs_selection_7_4.cnf"
CFG_PATH="$PWD/resources/config/ea/base.json"
SOLVE_BIN="$PWD/bazel-bin/evol/main"
TEST_BIN="$PWD/bazel-bin/evol/test"
VERBOSE="2"

while ! [[ -z "$1" ]]; do
    case "$1" in
        -h|--help) {
            echo "Usage: ./run.sh [options]"
            echo "  Options:"
            echo "   -h|--help           Display this message"
            echo "   -bd|--backdoor      Enable backdoor"
            echo "   -b|--build          Do build"
            echo "   -f|--format         Do reformat"
            echo "   -p|--proto          Do proto rebuild"
            echo "   -t|--test Do test"
            echo "   -v|--verbose <x>    Set verbose level to x[=2]"
            echo "   -c|--config <path>  Path to config"
            echo "   -i|--input <path>   Path to CNF"
            echo "   -s|--solve          Do solve"
            echo "   -a|--all            Do all (-b, -f, -p, -s, -t)"
        } ;;
        -bd|--backdoor) {
            BACKDOOR="--backdoor"
        } ;;
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
        -f|--format) {
            FORMAT="YES"
        } ;;
        -b|--build) {
            BUILD="YES"
        } ;;
        -p|--proto) {
            PROTO="YES"
        } ;;
        -s|--solve) {
            SOLVE="YES"
        } ;;
        -t|--test) {
            TEST="YES"
        } ;;
        -a|--all) {
            FORMAT="YES"
            BUILD="YES"
            SOLVE="YES"
            TEST="YES"
            PROTO="YES"
        } ;;
        *) {
            echo "Unkown option: $1"
            exit 1
        } ;;
    esac

    shift
done


if ! [[ -z "$FORMAT" ]]; then
    find . -iname *.h -o -iname *.hpp -o -iname *.cpp -o -iname *.cc | xargs clang-format -i
fi

if ! [[ -z "$PROTO" ]]; then
    rm ./evol/proto/config.pb.h || true
    rm ./evol/proto/config.pb.cc || true
    bazel build '//evol/proto:*'
    ln -s `pwd`/bazel-bin/evol/proto/config.pb.h ./evol/proto/config.pb.h
    ln -s `pwd`/bazel-bin/evol/proto/config.pb.cc ./evol/proto/config.pb.cc
fi

if ! [[ -z "$BUILD" ]]; then
    bazel build //evol:main //evol:test
fi

if ! [[ -z "$TEST" ]]; then
    GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $TEST_BIN
fi

if ! [[ -z "$SOLVE" ]]; then
    GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 $SOLVE_BIN \
        $BACKDOOR \
        --input "$CNF_PATH" \
        --config "$CFG_PATH"
fi

