sources = glob(include=[
    "src/**/*.cpp",
    "include/**/*.h",
], exclude=[
    "src/main.cpp",
])

pq_linkopts = [
    "-lpq", 
    "-lpqxx",
    "-L/usr/local/lib",
]

base_deps = [
    "@boost//:program_options",
    "@boost//:timer",
    "@boost//:asio",
    "@boost//:process",
    "@com_github_google_glog//:glog",
    "@com_google_protobuf//:protobuf",
    "@com_google_googletest//:gtest_main",
]

cc_binary(
    name = "main",
    srcs = sources + ["src/main.cpp"],
    deps = base_deps,
    linkopts = pq_linkopts,
)

cc_test(
    name = "test",
    size = "small",
    srcs = glob(["test/**/*.cpp"]) + sources,
    deps = base_deps + [
        "//infra:main",
    ],
    linkopts = pq_linkopts,
)
