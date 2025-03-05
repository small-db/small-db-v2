set(ARROW_BUILD_SHARED True)
set(ARROW_DEPENDENCY_SOURCE "BUNDLED")
set(ARROW_SIMD_LEVEL AVX512)

set(ARROW_ACERO ON)
set(ARROW_PARQUET ON)
set(ARROW_IPC ON)
set(ARROW_DATASET ON)
set(ARROW_FILESYSTEM ON)
set(ARROW_COMPUTE ON)

set(ARROW_BUILD_TESTS OFF)

include(FetchContent)

FetchContent_Declare(Arrow
  GIT_REPOSITORY https://github.com/apache/arrow.git
    GIT_TAG apache-arrow-19.0.1
  GIT_SHALLOW TRUE SOURCE_SUBDIR cpp
  OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(Arrow)