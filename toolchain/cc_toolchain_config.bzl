load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "feature",  # NEW
    "flag_group",  # NEW
    "flag_set",  # NEW
    "tool_path",
)

all_link_actions = [  # NEW
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]


def _impl(ctx):
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
            "/home/xiaochen/code/small-db-v2/src/query",  # Add this line
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


cc_toolchain_config = rule(
    implementation=_impl,
    attrs={},
    provides=[CcToolchainConfigInfo],
)
