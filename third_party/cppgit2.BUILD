load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)

cmake(
  name = "cppgit2",
  lib_source = ":srcs",
  visibility = ["//visibility:public"],
  copts = ["-fPIC"],
  generate_args = ["-DBUILD_SHARED_LIBS=OFF"],
  build_args = ["-j", "16"],
  #out_lib_dir = "../lib",
  out_static_libs = ["libcppgit2.a"],
)

