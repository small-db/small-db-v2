load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "feature",
    "flag_group",
    "flag_set",
    "tool_path",
)

all_link_actions = [  # NEW
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]


def impl_clang20(ctx):
    tool_paths = [
        tool_path(
            name="gcc",
            path="/usr/lib/llvm-20/bin/clang-20",
        ),
        tool_path(
            name="ld",
            path="/usr/lib/llvm-20/bin/ld.lld",
        ),
        tool_path(
            name="ar",
            path="/usr/lib/llvm-20/bin/llvm-ar",
        ),
        tool_path(
            name="cpp",
            path="/bin/false",
        ),
        tool_path(
            name="gcov",
            path="/bin/false",
        ),
        tool_path(
            name="nm",
            path="/bin/false",
        ),
        tool_path(
            name="objdump",
            path="/bin/false",
        ),
        tool_path(
            name="strip",
            path="/bin/false",
        ),
    ]

    features = [
        feature(
            name="default_linker_flags",
            enabled=True,
            flag_sets=[
                flag_set(
                    actions=all_link_actions,
                    flag_groups=(
                        [
                            flag_group(
                                flags=[
                                    "-lstdc++",
                                    "-lm",
                                ],
                            ),
                        ]
                    ),
                ),
            ],
        ),
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx=ctx,
        features=features,
        cxx_builtin_include_directories=[
            "/usr/lib/llvm-20/lib/clang/20/include",
            "/usr/include",
            # note that we must use absolute path here since clang embeds the file using absolute path
            "/home/xiaochen/code/small-db-v2/src/query",
        ],
        toolchain_identifier="k8-toolchain",
        host_system_name="local",
        target_system_name="local",
        target_cpu="k8",
        target_libc="unknown",
        compiler="clang",
        abi_version="unknown",
        abi_libc_version="unknown",
        tool_paths=tool_paths,
    )


cc_toolchain_config_clang20 = rule(
    implementation=impl_clang20,
    attrs={},
    provides=[CcToolchainConfigInfo],
)

def impl_gcc13(ctx):
    tool_paths = [
        tool_path(
            name="gcc",
            path="/usr/bin/gcc-12",
        ),
        tool_path(
            name="ld",
            path="/usr/bin/ld",
        ),
        tool_path(
            name="ar",
            path="/usr/bin/ar",
        ),
        tool_path(
            name="cpp",
            path="/bin/false",
        ),
        tool_path(
            name="gcov",
            path="/bin/false",
        ),
        tool_path(
            name="nm",
            path="/bin/false",
        ),
        tool_path(
            name="objdump",
            path="/bin/false",
        ),
        tool_path(
            name="strip",
            path="/bin/false",
        ),
    ]

    features = [
        feature(
            name="default_linker_flags",
            enabled=True,
            flag_sets=[
                flag_set(
                    actions=all_link_actions,
                    flag_groups=(
                        [
                            flag_group(
                                flags=[
                                    "-lstdc++",
                                    "-lm",
                                ],
                            ),
                        ]
                    ),
                ),
            ],
        ),
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx=ctx,
        features=features,
        cxx_builtin_include_directories=[
            "/usr/lib/llvm-20/lib/clang/20/include",
            "/usr/include",
            # note that we must use absolute path here since clang embeds the file using absolute path
            "/home/xiaochen/code/small-db-v2/src/query",
        ],
        toolchain_identifier="k8-toolchain",
        host_system_name="local",
        target_system_name="local",
        target_cpu="k8",
        target_libc="unknown",
        compiler="clang",
        abi_version="unknown",
        abi_libc_version="unknown",
        tool_paths=tool_paths,
    )


cc_toolchain_config_gcc13 = rule(
    implementation=impl_gcc13,
    attrs={},
    provides=[CcToolchainConfigInfo],
)