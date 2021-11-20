load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "srcs",
    srcs = glob(["**/*.cc", "**/*.h", "**/CMakeLists.txt"]),
    visibility = ["//visibility:public"],
)

cmake(
  name = "libminisat",
  lib_name = "libminisat",
  lib_source = "//minisat:srcs",
  visibility = ["//visibility:public"],
)

