add_library(small_catalog
    catalog.h
    catalog.cc
)

target_link_libraries(small_catalog
    PRIVATE
    spdlog::spdlog
)

add_library(small::catalog ALIAS small_catalog)
