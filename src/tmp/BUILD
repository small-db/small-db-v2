cc_binary(
    name = "log",
    srcs = ["log.cc"],
    deps = [
        "//:spdlog",
    ],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [
        "//:libpqxx",
    ],
)

cc_test(
    name = "hello_test",
    size = "small",
    srcs = ["hello_test.cc"],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
