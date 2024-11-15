cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "arrow_example",
    srcs = ["arrow_example.cc"],
    copts = ["-Iexternal/libarrow/src/include"],
    deps = ["@libarrow//:main"],
    visibility = ["//visibility:public"],
)

# cc_binary(
#     name = "server",
#     srcs = ["src/main.cc"],
#     copts = ["-Iexternal/liburing/src/include"],
#     deps = ["@liburing//:main"]
# )