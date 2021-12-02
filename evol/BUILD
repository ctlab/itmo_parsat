sources = glob(include=[
    "src/**/*.cpp",
    "src/**/*.cc",
    "include/**/*.h",
    "proto/**/*.h",
    "proto/**/*.cc",
], exclude=[
    "src/main.cpp",
])

cc_binary(
    name = "main",
    srcs = sources + ["src/main.cpp"],
    deps = [
        "@boost//:program_options",
        "@boost//:timer",
        "@boost//:asio",
        "@com_github_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
        "//evol/proto:config_cc_proto",
        "//minisat:libminisat",
    ],
    copts = ["-Iminisat"],
)

cc_test(
    name = "test",
    size = "small",
    srcs = glob(["test/**/*.cpp"]) + sources,
    deps = [
        "@boost//:program_options",
        "@boost//:asio",
        "@com_github_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
        "@com_google_googletest//:gtest_main",
        "//evol/proto:config_cc_proto",
        "//evol:main",
        "//minisat:libminisat",
    ],
)
