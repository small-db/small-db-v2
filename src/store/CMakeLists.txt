# ============================================================================== #
# library: store_lib
# ============================================================================== #

add_library(store_lib
    store.cc
    store.h
)

target_link_libraries(store_lib
    PUBLIC
    schema_lib
    spdlog
)