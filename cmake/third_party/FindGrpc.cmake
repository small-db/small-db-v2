# > cmake -S . -B ./cmake/build -DCMAKE_CXX_STANDARD=17 -DgRPC_INSTALL=ON -DCMAKE_INSTALL_PREFIX=~/.local -G Ninja
# > cmake --build ./cmake/build
# need sudo since zlib doesn't care about CMAKE_INSTALL_PREFIX
# https://github.com/madler/zlib/blob/develop/CMakeLists.txt
# > sudo cmake --build ./cmake/build --target install

FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG v1.71.0
    GIT_SHALLOW TRUE
)

get_all_targets(. BEFORE_TARGETS)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(grpc)

get_all_targets(. AFTER_TARGETS)
print_added_target(BEFORE_TARGETS AFTER_TARGETS)
