#!/bin/bash
set -eux

CNF_PATH="$PWD/resources/cnf/pancake_vs_selection_7_4.cnf"
CFG_PATH="$PWD/resources/config/ea/base.json"
BINARY="$PWD/bazel-bin/evol/main"

bazel build //evol:main


GLOG_minloglevel=0 GLOG_logtostderr=1 $BINARY \
    --backdoor \
    --input "$CNF_PATH" \
    --config "$CFG_PATH"


