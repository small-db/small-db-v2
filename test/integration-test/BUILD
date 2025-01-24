cc_test(
    name = "sql_test",
    size = "small",
    srcs = ["sql_test.cc"],
    deps = [
        # internal dependencies
        "//src/server:server",
        # external dependencies
        "//:spdlog",
        "//:libpqxx",
        # external dependencies from bazel central repository
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
