block()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
    FetchContent_Declare(
        libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx.git
        GIT_TAG 7.10.0
    )

    FetchContent_MakeAvailable(libpqxx)
endblock()
