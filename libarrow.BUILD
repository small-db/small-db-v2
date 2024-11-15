cc_library(
    name = "main",

    # https://bazel.build/reference/be/c-cpp#cc_library.srcs
    srcs = glob(
        [
            "cpp/src/arrow/**/*.cc",
            "cpp/src/arrow/**/*.h",
        ],
        exclude = [
            "**/*_test.cc",
            "**/*_benchmark.cc",
            "**/testing/**",
            "**/bpacking*.cc",
        ],
    ),

    # https://bazel.build/reference/be/c-cpp#cc_library.hdrs
    # hdrs = glob(["src/*.h", "src/include/*.h", "src/include/liburing/*.h"]),
    hdrs = glob(["cpp/src/arrow/*.h"]),
    # hdrs = glob(["cpp/src"]),
    # hdrs = ["cpp/src"],

    # copts = ["-Iexternal/liburing/src/include"],
    copts = ["-Iexternal/libarrow/cpp/src"],
    # copts = ["-Iexternal/libarrow/cpp/src", "-Iexternal/libarrow/cpp/build/src"],
    visibility = ["//visibility:public"],
)
