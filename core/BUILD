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
    deps = base_deps + [
        "@com_google_googletest//:gtest_main",
        "//core/domain:domain",
    ],
    visibility = ["//visibility:public"],
)
