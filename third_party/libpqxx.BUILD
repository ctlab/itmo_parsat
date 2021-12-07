load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)

cmake(
  name = "libpqxx",
  lib_name = "libpqxx",
  lib_source = ":srcs",
  visibility = ["//visibility:public"],
  out_static_libs = ["libpqxx.lib"]
)

