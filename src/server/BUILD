cc_binary(
    name = "main",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":server",

        # internal dependencies
        "//src/query",
        "//src/store",

        # external dependencies
        "//:libpg_query",
        "//:spdlog",
    ],
)

cc_library(
    name = "server",
    srcs = [
        "args.cc",
        "args.h",
        "server.cc",
        "server.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:libpg_query",
        "//:spdlog",
        "//src/query",
        "//src/store",
    ],
)
