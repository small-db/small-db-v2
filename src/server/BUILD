cc_binary(
    name = "main",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//:libpg_query",
        "//:spdlog",
        "//src/query",
        "//src/store",
    ],
)

cc_library(
    name = "server",
    srcs = [
        "args.cc",
        "main.cc",
        "server.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:libpg_query",
        "//:spdlog",
        "//src/query",
        "//src/store",
    ],
)
