workspace(name = "smalldb")

# ================================================================================ #
# http_archive
# ================================================================================ #
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

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
    build_file_content = _ALL_CONTENT,
    strip_prefix = "arrow-apache-arrow-18.0.0",
    urls = ["https://github.com/apache/arrow/archive/refs/tags/apache-arrow-18.0.0.tar.gz"],
)

http_archive(
    name = "rocksdb",
    build_file_content = _ALL_CONTENT,
    strip_prefix = "rocksdb-9.7.4",
    urls = ["https://github.com/facebook/rocksdb/archive/refs/tags/v9.7.4.tar.gz"],
)
