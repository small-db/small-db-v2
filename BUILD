# ====================================================================================== #
# external dependencies
# ====================================================================================== #
load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake", "configure_make", "make")

cmake(
    name = "arrow",
    build_args = ["-j"],
    cache_entries = {
        "CMAKE_INSTALL_LIBDIR": "lib",
        "CMAKE_TOOLCHAIN_FILE": "",
        "CMAKE_C_FLAGS": "-fPIC -I/usr/include -fvisibility=hidden",
        "CMAKE_CXX_FLAGS": "-fPIC -I/usr/include -fvisibility=hidden",
        "EP_COMMON_CMAKE_ARGS": "-DWITH_OPENSSL=OFF",
        "ARROW_ACERO": "ON",
        "ARROW_BUILD_SHARED": "OFF",
        "ARROW_BUILD_STATIC": "ON",
        "ARROW_BUILD_TESTS": "OFF",
        "ARROW_BUILD_UTILITIES": "ON",
        "ARROW_DATASET": "ON",
        "ARROW_PARQUET": "ON",
        "ARROW_JEMALLOC": "OFF",
        "ARROW_IPC": "OFF",
        "ARROW_DEPENDENCY_SOURCE": "BUNDLED",
        "ARROW_WITH_SNAPPY": "ON",
        "ARROW_WITH_ZSTD": "ON",
        "ARROW_SUBSTRAIT": "ON",
        "CMAKE_OSX_DEPLOYMENT_TARGET": "12",
        # The BUILD_WARNING_LEVEL CMake option switches between sets of predetermined compiler warning levels that we use for code tidiness. For release builds, the default warning level is PRODUCTION, while for debug builds the default is CHECKIN.
        # When using CHECKIN for debug builds, -Werror is added when using gcc and clang, causing build failures for any warning, and /WX is set with MSVC having the same effect.
        # see:
        # https://arrow.apache.org/docs/developers/cpp/development.html#compiler-warning-levels
        "BUILD_WARNING_LEVEL": "PRODUCTION",
    },
    generate_args = [
        "-GNinja",
        "-DCMAKE_RANLIB=/usr/bin/ranlib",
    ],
    lib_source = "@arrow//:all",
    linkopts = ["-pthread"],

    # These files will be put into dozens of directories, one of which is:
    # ./bazel-bin/arrow/lib/
    out_static_libs = [
        "libarrow_substrait.a",
        "libarrow_acero.a",
        "libarrow_dataset.a",
        "libparquet.a",
        "libarrow.a",
        "libarrow_bundled_dependencies.a",
    ],
    tags = ["requires-network"],
    visibility = ["//visibility:public"],
    working_directory = "cpp",
)

cmake(
    name = "rocksdb",
    build_args = ["-j"],
    generate_args = [
        "-DWITH_GFLAGS=OFF",
        "-DROCKSDB_BUILD_SHARED=OFF",
    ],
    lib_source = "@rocksdb//:all",
    out_static_libs = ["librocksdb.a"],
    targets = ["rocksdb"],
    visibility = ["//visibility:public"],
)

cmake(
    name = "spdlog",
    build_args = ["-j"],
    lib_source = "@spdlog//:all",
    out_static_libs = select({
        "//conditions:default": ["libspdlog.a"],

        # spdlog produces a library with "d" postfix when building in debug mode.
        #
        # The ideal approach is to use CMAKE_DEBUG_POSTFIX so the library name is consistent with the default one, but spdlog doesn't support this option. So we have to adapt to it.
        ":debug": ["libspdlogd.a"],
    }),
    visibility = ["//visibility:public"],
)

configure_make(
    name = "postgres",
    args = ["-j"],
    # sudo apt install -y bison flex
    # ./configure --without-readline --without-zlib --without-icu
    configure_options = [
        "--without-readline",
        "--without-zlib",
        "--without-icu",
    ],
    includes = ["server"],
    lib_source = "@postgres//:all",
    out_shared_libs = [
        "libpq.so",
    ],
    # out_static_libs = [
    #     # not found for some reason
    #     # "libpgport_srv.a",
    #     "libpgport_shlib.a",
    #     "libpgport.a",
    #     "libpgcommon.a",
    #     "libpgcommon_shlib.a",
    #     # not found for some reason
    #     # "libpgcommon_srv.a",
    #     "libpgtypes.a",
    #     "libpq.a",
    #     "libecpg.a",
    #     "libpgfeutils.a",
    #     "libecpg_compat.a",
    # ],
    visibility = ["//visibility:public"],
)

make(
    name = "libpg_query",
    lib_source = "@libpg_query//:all",
    out_static_libs = ["libpg_query.a"],
    targets = [
        "libpg_query.a",
        "install",
    ],
    visibility = ["//visibility:public"],
)

cmake(
    name = "libpqxx",
    build_args = ["-j"],
    lib_source = "@libpqxx//:all",
    # generate_args = [
    #     "-DBUILD_SHARED_LIBS=off",
    # ],
    out_static_libs = ["libpqxx.a"],
    visibility = ["//visibility:public"],
    deps = [
        # libpqxx depends on libpq which is built from postgres
        ":postgres",
    ],
)

config_setting(
    name = "debug",
    values = {"compilation_mode": "dbg"},
)

# ====================================================================================== #
# top-level targets
# ====================================================================================== #
test_suite(
    name = "integration_test",
    tests = [
        "//test/integration_test:sql_test",
    ],
)
