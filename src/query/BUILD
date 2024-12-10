cc_library(
    name = "query",
    srcs = [
        "query.cc",
        # "embed_example.cc",
        # "xc_simple_substrait.json",
    ],
    hdrs = ["query.h"],
    # data = ["simple.json"],
    # additional_compiler_inputs = ["simple.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//:arrow",
        "//:spdlog",
        # "//battery:embed",
    ],
)
