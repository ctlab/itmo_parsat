cc_library(
    name = "evol",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
    deps = [
        "@boost//:program_options",
        "@boost//:timer",
        "@boost//:asio",
        "@com_github_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
        "//core/proto:config_cc_proto",
        "//core/sat:sat"
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

