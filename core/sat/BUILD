cc_library(
    name = "sat",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//core/util:util",
        "//core/domain:domain",
        "//core/proto:config_cc_proto",
        "//core/sat/minisat:minisat",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"],
)