add_library(small_kv_store
    kv_store.h
    kv_store.cc
)

target_link_libraries(small_kv_store
    PRIVATE
    spdlog::spdlog
    small::type
    small::encode
    small::schema
)

add_library(small::kv_store ALIAS small_kv_store)
