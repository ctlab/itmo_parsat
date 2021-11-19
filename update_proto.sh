#!/bin/bash
set -eux

BAZEL_CXXOPTS="-std=c++17" bazel build //evol/proto:*

rm ./evol/proto/config.pb.h || true
rm ./evol/proto/config.pb.cc || true

ln -s `pwd`/bazel-bin/evol/proto/config.pb.h ./evol/proto/config.pb.h
ln -s `pwd`/bazel-bin/evol/proto/config.pb.cc ./evol/proto/config.pb.cc
