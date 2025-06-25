cmake_minimum_required(VERSION 3.5.0)

project(
    perfect_hash_table
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
add_subdirectory(bin)