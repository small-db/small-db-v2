workspace(name = "smalldb")

# ================================================================================ #
# http_archive
# ================================================================================ #
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
# load("@bazel_tools//tools/build_defs/repo:local.bzl", "new_local_repository")

# ================================================================================ #
# rules_foreign_cc lets us build C/C++ projects using cmake/make/etc.
# ================================================================================ #
http_archive(
    name = "rules_foreign_cc",
    sha256 = "a2e6fb56e649c1ee79703e99aa0c9d13c6cc53c8d7a0cbb8797ab2888bbc99a3",
    strip_prefix = "rules_foreign_cc-0.12.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.12.0/rules_foreign_cc-0.12.0.tar.gz",
)
# local_repository(
#     name = "rules_foreign_cc",
#     path = "/home/xiaochen/code/rules_foreign_cc",
# )

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
#   "external/liburing"
# With MODULE.bazel, the path becomes:
#   "external/_main~_repo_rules~liburing"
# ================================================================================ #

# A generic content for all http_archive rules.
_ALL_CONTENT = """\
filegroup(
    name = "all",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""

# http_archive(
#     name = "arrow",
#     build_file_content = _ALL_CONTENT,
#     strip_prefix = "arrow-apache-arrow-18.0.0",
#     urls = ["https://github.com/apache/arrow/archive/refs/tags/apache-arrow-18.0.0.tar.gz"],
# )

# http_archive(
#     name = "rocksdb",
#     build_file_content = _ALL_CONTENT,
#     strip_prefix = "rocksdb-9.7.4",
#     urls = ["https://github.com/facebook/rocksdb/archive/refs/tags/v9.7.4.tar.gz"],
# )

# http_archive(
#     name = "spdlog",
#     build_file_content = _ALL_CONTENT,
#     strip_prefix = "spdlog-1.15.0",
#     urls = ["https://github.com/gabime/spdlog/archive/refs/tags/v1.15.0.tar.gz"],
# )

# http_archive(
#     name = "postgres",
#     build_file_content = _ALL_CONTENT,
#     strip_prefix = "postgres-REL_17_1",
#     urls = ["https://github.com/postgres/postgres/archive/refs/tags/REL_17_1.tar.gz"],
# )

# http_archive(
#     name = "libpg_query",
#     build_file_content = _ALL_CONTENT,
#     # strip_prefix = "libpg_query-17-6.0.0",
#     # urls = ["https://github.com/pganalyze/libpg_query/archive/refs/tags/17-6.0.0.tar.gz"],
#     strip_prefix = "libpg_query-xiaochen-17-6.0.0",
#     # we use a forked version of libpg_query since the origin's "make install" doesn't work
#     # with bazel
#     urls = ["https://github.com/small-db/libpg_query/archive/refs/tags/xiaochen-17-6.0.0.tar.gz"],
# )

# # we use a forked version of libpg_query since the origin's "make install" doesn't work
# # with bazel
# http_archive(
#     name = "libpg_query",
#     build_file_content = _ALL_CONTENT,
#     # strip_prefix = "libpg_query-17-6.0.0",
#     # urls = ["https://github.com/pganalyze/libpg_query/archive/refs/tags/17-6.0.0.tar.gz"],
#     strip_prefix = "libpg_query-xiaochen-17-6.0.0",
#     urls = ["https://github.com/small-db/libpg_query/archive/refs/tags/xiaochen-17-6.0.0.tar.gz"],
# )

# new_local_repository(
#     name = "libpg_query",
#     build_file_content = _ALL_CONTENT,
#     path = "../libpg_query",
# )

