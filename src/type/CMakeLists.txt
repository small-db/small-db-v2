add_library(type_lib
    type.cc
    type.h
)

target_link_libraries(type_lib
    PRIVATE
    base_lib
)
