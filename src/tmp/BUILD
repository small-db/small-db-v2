cc_binary(
    name = "log",
    srcs = ["log.cc"],
    deps = [
        "//:spdlog",
    ],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
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
