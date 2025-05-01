add_library(small_pg_wire
    pg_wire.h
    pg_wire.cc
)

target_link_libraries(small_pg_wire
    PRIVATE
    arrow_lib
)

add_library(small::pg_wire ALIAS small_pg_wire)
