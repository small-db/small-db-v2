add_library(file_system_lib
    file_system.cc
    file_system.h
)

target_link_libraries(file_system_lib
    PRIVATE
    base_lib
)
