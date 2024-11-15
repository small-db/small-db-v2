workspace(name = "templates")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# ================================================================================ #
# External Dependencies
#
# We manage external dependencies with WORKSPACE instead of MODULE.bazel (Bzlmod)
# because MODULE.bazel creates a more complex library path.
#
# For example, when importing the external library "liburing" using WORKSPACE,
# the library path is:
#   "bazel-templates/external/liburing"
# With MODULE.bazel, the path becomes:
#   "bazel-templates/external/_main~_repo_rules~liburing"
# ================================================================================ #

http_archive(
    name = "libarrow",

    # https://bazel.build/rules/lib/repo/http#http_archive-build_file
    #
    # Once the "build_file" is updated, bazel will re-run the download and patch process.
    build_file = "@//:libarrow.BUILD",

    # https://bazel.build/rules/lib/repo/http#http_archive-patch_cmds
    #
    # Tips:
    # - These commands run in separate shell environments, so "cd" command won't affect the following command.
    #
    # arrow's build manual:
    # https://arrow.apache.org/docs/developers/cpp/building.html#
    patch_cmds = [
        "mkdir cpp/build",
        "cd cpp/build && cmake .. --preset ninja-debug-minimal",
        "cd cpp/build && cmake --build .",
    ],

    # https://bazel.build/rules/lib/repo/http#http_archive-strip_prefix
    #
    # Tips:
    # - It doesn't support multiple layers of directories (e.g., "arrow-apache-arrow-18.0.0/cpp").
    strip_prefix = "arrow-apache-arrow-18.0.0",
    urls = ["https://github.com/apache/arrow/archive/refs/tags/apache-arrow-18.0.0.tar.gz"],
)
