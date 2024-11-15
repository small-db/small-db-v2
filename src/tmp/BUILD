cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "arrow_example",
    srcs = ["arrow_example.cc"],
    # copts = ["-Iexternal/libarrow/cpp/src"],
    # copts = ["-Iexternal/libarrow/cpp/src", "-Iexternal/libarrow/cpp/build/src"],
    # deps = ["@arrow//:main"],
    # deps = ["@arrow//:arrow_cmake"],
    deps = ["//:arrow"],
    visibility = ["//visibility:public"],
)

# cc_binary(
#     name = "server",
#     srcs = ["src/main.cc"],
#     copts = ["-Iexternal/liburing/src/include"],
#     deps = ["@liburing//:main"]
# )