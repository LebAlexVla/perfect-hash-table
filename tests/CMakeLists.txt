include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
)

set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(googlebenchmark)

enable_testing()

add_executable(
    perfect_hash_table_tests
    PerfectHashTableTest.cpp
)

target_link_libraries(
    perfect_hash_table_tests
    perfect_hash_table_lib
    GTest::gtest_main
)

target_include_directories(perfect_hash_table_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(perfect_hash_table_tests)

add_executable(
    perfect_hash_table_benchmark
    PerfectHashTableBenchmark.cpp
)

target_link_libraries(
    perfect_hash_table_benchmark
    perfect_hash_table_lib
    benchmark::benchmark
)

target_include_directories(perfect_hash_table_benchmark PUBLIC ${PROJECT_SOURCE_DIR})