cc_binary(
    name = "hello-boost",
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
  size = "small",
  srcs = glob(["test/**/*.cpp"]),
  deps = ["@com_google_googletest//:gtest_main"],
)
