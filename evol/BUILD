cc_binary(
    name = "main",
    copts = ["-std=c++17"],
    srcs = glob(["src/**/*.cpp", "include/**/*.h"]),
    deps = [
        "@boost//:throw_exception",
        "@boost//:assert",
        "@boost//:cstdint",
        "@com_github_nlohmann_json//:json",
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
  name = "hello_test",
  copts = ["-std=c++17"],
  size = "small",
  srcs = glob(["test/**/*.cpp"]),
  deps = ["@com_google_googletest//:gtest_main"],
)
