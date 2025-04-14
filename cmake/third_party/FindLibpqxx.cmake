block()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

    set(CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_BUILD_TYPE})

    FetchContent_Declare(
        libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx.git
        GIT_TAG 7.10.0
    )

    # result config header:
    # ./build/debug/_deps/libpqxx-build/include/pqxx/config-public-compiler.h
    # 
    # default content:
    # ...
    # #define PQXX_HAVE_ASSUME
    # ...
    # #define PQXX_HAVE_GCC_PURE
    # ...

    FetchContent_MakeAvailable(libpqxx)
endblock()
