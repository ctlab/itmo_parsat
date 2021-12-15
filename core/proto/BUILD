load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_proto_library(
    name = "solve_config_cc_proto",
    deps = [":solve_config_proto"],
    visibility=["//visibility:public"],
)

cc_proto_library(
    name = "logging_config_cc_proto",
    deps = [":logging_config_proto"],
    visibility=["//visibility:public"],
)

proto_library(
    name = "solve_config_proto",
    srcs = ["solve_config.proto"],
    deps = [
        "@com_google_protobuf//:any_proto",
    ],
)

proto_library(
    name = "logging_config_proto",
    srcs = ["logging_config.proto"],
    deps = [
        "@com_google_protobuf//:any_proto",
    ],
)

