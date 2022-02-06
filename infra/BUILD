cc_library(
    name = "infra",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
    deps = [
        "@boost//:program_options",
        "@boost//:timer",
        "@boost//:asio",
        "@boost//:process",
        "@com_github_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
        "@com_google_googletest//:gtest_main",
        "@libpqxx//:libpqxx",
        "//cli:solve",
    ],
    linkopts = [
        "-lpq",
        #"-lpqxx"
    ],
    alwayslink = True,
    linkstatic = True,
    visibility = ["//visibility:public"]
)

