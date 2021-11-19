cc_binary(
    name = "main",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.cc",
        "proto/**/*.cc",
        "proto/**/*.h",
        "include/**/*.h",
    ]),
    deps = [
        "@boost//:program_options",
        "@com_github_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
        "//evol/proto:config_cc_proto",
    ],
)

cc_library(
    name = "evol",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.cc",
        "proto/**/*.cc",
        "proto/**/*.h",
        "include/**/*.h",
    ]),
    deps = [
        "@boost//:program_options",
        "@com_github_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
        "//evol/proto:config_cc_proto",
    ],
)

cc_test(
  name = "hello_test",
  size = "small",
  srcs = glob(["test/**/*.cpp"]),
  deps = ["@com_google_googletest//:gtest_main"],
)
