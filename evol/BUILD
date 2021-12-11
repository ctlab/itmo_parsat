sources = glob(include=[
    "src/**/*.cpp",
    "src/**/*.cc",
    "include/**/*.h",
    "proto/**/*.h",
    "proto/**/*.cc",
], exclude=[
    "src/main.cpp",
])

base_deps = [
    "@boost//:program_options",
    "@boost//:timer",
    "@boost//:asio",
    "@com_github_google_glog//:glog",
    "@com_google_protobuf//:protobuf",
    "//evol/proto:config_cc_proto",
    "//minisat:libminisat",
]

cc_binary(
    name = "main",
    srcs = sources + ["src/main.cpp"],
    deps = base_deps,
    copts = ["-Iminisat"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test",
    size = "small",
    srcs = glob(["test/**/*.cpp"]) + sources,
    deps = base_deps + [
        "@com_google_googletest//:gtest_main",
        "//evol:main",
    ],
    visibility = ["//visibility:public"],
)
