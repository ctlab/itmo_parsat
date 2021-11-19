load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_proto_library(
    name = "config_cc_proto",
    deps = [":config_proto"],
    visibility=["//visibility:public"],
)

proto_library(
    name = "config_proto",
    srcs = ["config.proto"],
    deps = [
        "@com_google_protobuf//:any_proto",
    ],
    visibility=["//visibility:public"],
)