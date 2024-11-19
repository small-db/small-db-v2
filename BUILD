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
        "ARROW_BUILD_SHARED": "OFF",
        "ARROW_BUILD_STATIC": "ON",
        "ARROW_BUILD_TESTS": "OFF",
        "ARROW_PARQUET": "ON",
        "ARROW_JEMALLOC": "OFF",
        "ARROW_IPC": "OFF",
        "ARROW_DEPENDENCY_SOURCE": "BUNDLED",
        "ARROW_WITH_SNAPPY": "ON",
        "ARROW_WITH_ZSTD": "ON",
        "CMAKE_OSX_DEPLOYMENT_TARGET": "12",
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

        # spdlog doesn't care about CMAKE_DEBUG_POSTFIX, so we have to adapt to it.
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
    out_static_libs = [
        "libpq.a",
        "libpgcommon.a",
        "libpgport.a",
        "libpgport_shlib.a",
    ],
    visibility = ["//visibility:public"],
)

make(
    name = "libpg_query",
    args = [
        "-j",
    ],
    # Specify the target since the default target will run "make install" and fail.
    lib_source = "@libpg_query//:all",
    out_lib_dir = "",
    out_static_libs = ["libpg_query.a"],
    targets = ["libpg_query.a"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "debug",
    values = {"compilation_mode": "dbg"},
)
