cc_library(
  name = "minisat",
  srcs = glob(include=["minisat/**/*.cc"], exclude=["minisat/**/Main.cc"]),
  hdrs = glob(["minisat/**/*.h"]),
  copts = ["-Icore/sat/minisat", "-w", "-Ofast", "-march=native"],
  alwayslink = True,
  linkstatic = True,
  visibility = ["//visibility:public"],
)
