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
        "//core:domain",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sat",
    srcs = glob(["sat/**/*.cpp"]),
    hdrs = glob(["sat/**/*.h"]),
    deps = [
        "//core:util",
        "//core:domain",
        "//core/proto:solve_config_cc_proto",
        "//core/sat/minisat:minisat",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "util",
    srcs = glob(["util/**/*.cpp"]),
    hdrs = glob(["util/**/*.h"]),
    deps = base_deps + [
        "//core/proto:logging_config_cc_proto"
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "logging",
    srcs = glob(["logging/**/*.cpp"]),
    hdrs = glob(["logging/**/*.h"]),
    deps = base_deps,
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "evol",
    srcs = glob(["evol/**/*.cpp"]),
    hdrs = glob(["evol/**/*.h"]),
    deps = base_deps + [
        "//core/proto:solve_config_cc_proto",
        "//core:sat"
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "domain",
    srcs = glob(["domain/**/*.cpp"]),
    hdrs = glob(["domain/**/*.h"]),
    deps = [
        "//core:util",
        "//core/sat/minisat:minisat",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "solve",
    srcs = glob(["solve/**/*.cpp"]),
    hdrs = glob(["solve/**/*.h"]),
    deps = [
        "//core:evol",
        "//core:sat",
        "//core:util",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)
