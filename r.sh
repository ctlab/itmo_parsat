#!/bin/bash
set -eux

BAZEL_CXXOPTS="-std=c++17" bazel build //evol:* --compilation_mode=dbg -s
GLOG_logtostderr=1 GLOG_minloglevel=0 ./bazel-bin/evol/main

