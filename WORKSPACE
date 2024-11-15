workspace(name = "smalldb")

# ================================================================================ #
# http_archive
# ================================================================================ #
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

# http_archive(
#     name = "libarrow",

#     # https://bazel.build/rules/lib/repo/http#http_archive-build_file
#     #
#     # Once the content of the "build_file" is updated, bazel will re-run the download and patch process.
#     build_file = "@//:libarrow.BUILD",

#     # https://bazel.build/rules/lib/repo/http#http_archive-patch_cmds
#     #
#     # Tips:
#     # - These commands run in separate shell environments, so "cd" command won't affect the following command.
#     #
#     # arrow's build manual:
#     # https://arrow.apache.org/docs/developers/cpp/building.html#
#     patch_cmds = [
#         "mkdir cpp/build",
#         "cd cpp/build && cmake .. --preset ninja-debug-minimal",
#         "cd cpp/build && cmake --build .",
#     ],

#     # https://bazel.build/rules/lib/repo/http#http_archive-strip_prefix
#     #
#     # Tips:
#     # - It doesn't support multiple layers of directories (e.g., "arrow-apache-arrow-18.0.0/cpp").
#     strip_prefix = "arrow-apache-arrow-18.0.0",
#     urls = ["https://github.com/apache/arrow/archive/refs/tags/apache-arrow-18.0.0.tar.gz"],
# )

# ================================================================================ #
# rules_foreign_cc lets us build C/C++ projects using cmake/make/etc.
# ================================================================================ #
http_archive(
    name = "rules_foreign_cc",
    sha256 = "a2e6fb56e649c1ee79703e99aa0c9d13c6cc53c8d7a0cbb8797ab2888bbc99a3",
    strip_prefix = "rules_foreign_cc-0.12.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.12.0/rules_foreign_cc-0.12.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.12.0/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

load("@bazel_features//:deps.bzl", "bazel_features_deps")

bazel_features_deps()

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

# Group the sources of the library so that CMake rule have access to it
_ALL_CONTENT = """\
filegroup(
    name = "all",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""


http_archive(
    name = "arrow",

    # https://bazel.build/rules/lib/repo/http#http_archive-build_file
    #
    # Once the content of the "build_file" is updated, bazel will re-run the download and patch process.
    # build_file = _ALL_CONTENT,
    # build_file = "@//:libarrow.BUILD",
    build_file_content = _ALL_CONTENT,

    # https://bazel.build/rules/lib/repo/http#http_archive-patch_cmds
    #
    # Tips:
    # - These commands run in separate shell environments, so "cd" command won't affect the following command.
    #
    # arrow's build manual:
    # https://arrow.apache.org/docs/developers/cpp/building.html#
    # patch_cmds = [
    #     "mkdir cpp/build",
    #     "cd cpp/build && cmake .. --preset ninja-debug-minimal",
    #     "cd cpp/build && cmake --build .",
    # ],

    # https://bazel.build/rules/lib/repo/http#http_archive-strip_prefix
    #
    # Tips:
    # - It doesn't support multiple layers of directories (e.g., "arrow-apache-arrow-18.0.0/cpp").
    strip_prefix = "arrow-apache-arrow-18.0.0",
    urls = ["https://github.com/apache/arrow/archive/refs/tags/apache-arrow-18.0.0.tar.gz"],
)
