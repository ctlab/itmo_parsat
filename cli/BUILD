base_deps = [
    "@boost//:program_options",
    "@boost//:timer",
    "@boost//:asio",
    "@com_github_google_glog//:glog",
    "@com_google_protobuf//:protobuf",
    "//core:core",
]

cc_binary(
    name = "solve",
    srcs = glob(["solve/**/*.cpp", "solve/**/*.h"]),
    deps = base_deps,
    copts = ["-Icore/sat/minisat"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "infra",
    srcs = glob(["infra/**/*.cpp", "infra/**/*.h"]),
    deps = base_deps + [
        "//infra:infra"
    ],
    visibility = ["//visibility:public"],
)
