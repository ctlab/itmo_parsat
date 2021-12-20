package(default_visibility = ["//visibility:public"])

base_deps = [
    "@boost//:program_options",
    "@boost//:timer",
    "@boost//:asio",
    "@com_github_google_glog//:glog",
    "@com_google_protobuf//:protobuf",
]

cc_test(
    name = "test",
    size = "small",
    srcs = glob(["tests/**/*.cpp"]),
    copts = ["-Icore/sat/minisat"],
    deps = base_deps + [
        "@com_google_googletest//:gtest_main",
        "@com_google_googletest//:gtest",
        "//core:core",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "core",
    srcs = glob(include=["**/*.cpp"], exclude=["tests/**/*"]),
    hdrs = glob(include=["**/*.h"], exclude=["tests/**/*"]),
    deps = base_deps + [
        "//core/proto:logging_config_cc_proto",
        "//core/proto:solve_config_cc_proto",
        "//core/sat/minisat:minisat",
    ],
    copts = ["-Icore/sat/minisat"],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)
