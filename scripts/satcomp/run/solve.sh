#!/bin/bash

CONFIG="ps-sc.json"
CNF_PATH="$1"

GLOG_logtostderr=1 /itmo-parsat/build/cli/solve_bin \
    --print-model \
    --verbose 6 \
    --input "$CNF_PATH" \
    --solve-config "/itmo-parsat/resources/config/satcomp/$CONFIG" \
    --log-config "/itmo-parsat/resources/config/log.json"

