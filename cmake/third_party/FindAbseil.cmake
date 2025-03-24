FetchContent_Declare(Abseil
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG lts_2025_01_27
  GIT_SHALLOW TRUE
  SOURCE_SUBDIR cpp
  OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(Abseil)