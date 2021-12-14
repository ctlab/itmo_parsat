base_deps = [
    "@boost//:program_options",
    "@boost//:timer",
    "@boost//:asio",
    "@com_github_google_glog//:glog",
    "@com_google_protobuf//:protobuf",
]

cc_binary(
    name = "solve",
    srcs = glob(["solve/**/*.cpp", "solve/**/*.h"]),
    deps = base_deps + [
        "//evol:evol",
    ],
)