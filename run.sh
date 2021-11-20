#!/bin/bash
set -eux

CNF_PATH="$PWD/resources/cnf/pancake_vs_selection_7_4.cnf"
EA_CFG_PATH="$PWD/resources/config/ea/base.json"
MS_CFG_PATH="$PWD/resources/config/minisat/full_minisat_config.json"
BINARY="$PWD/bazel-bin/evol/main"

bazel build //evol:main


GLOG_minloglevel=0 GLOG_logtostderr=1 $BINARY \
    --backdoor \
    --input "$CNF_PATH" \
    --minisat-config "$MS_CFG_PATH" \
    --ea-config "$EA_CFG_PATH"


