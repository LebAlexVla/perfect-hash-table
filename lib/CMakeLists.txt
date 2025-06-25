add_library(
    perfect_hash_table_lib
    UniversalHash.cpp
)

target_include_directories(
    perfect_hash_table_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)