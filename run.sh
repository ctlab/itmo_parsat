#!/bin/bash
set -ex

CNF_PATH="$PWD/resources/cnf/pancake_vs_selection_7_4.cnf"
CFG_PATH="$PWD/resources/config/ea/base.json"
BINARY="$PWD/bazel-bin/evol/main"
VERBOSE="2"

while ! [[ -z "$1" ]]; do
    case "$1" in
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
        -s|--solve) {
            SOLVE="YES"
        } ;;
        -a|--all) {
            FORMAT="YES"
            BUILD="YES"
            SOLVE="YES"
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

if ! [[ -z "$BUILD" ]]; then
    bazel build //evol:main
fi

if ! [[ -z "$SOLVE" ]]; then
    GLOG_v=$VERBOSE GLOG_minloglevel=0 GLOG_logtostderr=1 \
        $BINARY \
        $BACKDOOR \
        --input "$CNF_PATH" \
        --config "$CFG_PATH"
fi


