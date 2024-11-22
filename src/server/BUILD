cc_binary(
    name = "main",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//:libpg_query",
        "//:spdlog",

        "//src/tmp:parquet_rw",
    ],
)
