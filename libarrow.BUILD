cc_library(
    name = "main",
    srcs = glob(["src/*.c"]),

    # hdrs = glob(["src/*.h", "src/include/*.h", "src/include/liburing/*.h"]),
    hdrs = glob(["cpp/**/*.h"]),

    # copts = ["-Iexternal/liburing/src/include"],
    copts = ["-Iexternal/libarrow/cpp/src/arrow/csv", "-larrow"],
    
    visibility = ["//visibility:public"]
)