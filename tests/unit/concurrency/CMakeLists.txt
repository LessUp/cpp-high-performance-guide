# Concurrency unit tests

add_executable(concurrency_utils_test
    concurrency_utils_test.cpp
)

target_include_directories(concurrency_utils_test PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/05-concurrency/include
)

target_link_libraries(concurrency_utils_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

hpc_set_compiler_options(concurrency_utils_test)
hpc_enable_sanitizers(concurrency_utils_test)

gtest_add_tests(TARGET concurrency_utils_test)
