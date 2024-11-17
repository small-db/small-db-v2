workspace(name = "smalldb")

# ================================================================================ #
# http_archive
# ================================================================================ #
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# ================================================================================ #
# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
# ================================================================================ #
http_archive(
    name = "hedron_compile_commands",
    strip_prefix = "bazel-compile-commands-extractor-0e990032f3c5a866e72615cf67e5ce22186dcb97",

    # Replace the commit hash (0e990032f3c5a866e72615cf67e5ce22186dcb97) in both places (below) with the latest (https://github.com/hedronvision/bazel-compile-commands-extractor/commits/main), rather than using the stale one here.
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/0e990032f3c5a866e72615cf67e5ce22186dcb97.tar.gz",
    # When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'hedron_compile_commands' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

load("@hedron_compile_commands//:workspace_setup_transitive.bzl", "hedron_compile_commands_setup_transitive")

hedron_compile_commands_setup_transitive()

load("@hedron_compile_commands//:workspace_setup_transitive_transitive.bzl", "hedron_compile_commands_setup_transitive_transitive")

hedron_compile_commands_setup_transitive_transitive()

load("@hedron_compile_commands//:workspace_setup_transitive_transitive_transitive.bzl", "hedron_compile_commands_setup_transitive_transitive_transitive")

hedron_compile_commands_setup_transitive_transitive_transitive()

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

http_archive(
    name = "spdlog",
    build_file_content = _ALL_CONTENT,
    strip_prefix = "spdlog-1.15.0",
    urls = ["https://github.com/gabime/spdlog/archive/refs/tags/v1.15.0.tar.gz"],
)

# https://github.com/postgres/postgres/archive/refs/tags/REL_17_1.tar.gz
http_archive(
    name = "postgres",
    build_file_content = _ALL_CONTENT,
    strip_prefix = "postgres-REL_17_1",
    urls = ["https://github.com/postgres/postgres/archive/refs/tags/REL_17_1.tar.gz"],
)