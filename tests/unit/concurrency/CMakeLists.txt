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
    hpc_headers
)

hpc_set_compiler_options(concurrency_utils_test)
hpc_enable_sanitizers(concurrency_utils_test)

gtest_add_tests(TARGET concurrency_utils_test)

# Lock-free queue tests
add_executable(lock_free_queue_test
    lock_free_queue_test.cpp
)

target_include_directories(lock_free_queue_test PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/05-concurrency/include
)

target_link_libraries(lock_free_queue_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
    hpc_headers
)

hpc_set_compiler_options(lock_free_queue_test)
hpc_enable_sanitizers(lock_free_queue_test)

gtest_add_tests(TARGET lock_free_queue_test)

