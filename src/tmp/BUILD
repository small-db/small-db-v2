cc_binary(
    name = "main",
    srcs = ["log.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//:spdlog",
    ],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
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
