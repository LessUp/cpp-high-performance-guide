# Memory unit tests

add_executable(memory_utils_test
    memory_utils_test.cpp
)

target_include_directories(memory_utils_test PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/02-memory-cache/include
)

target_link_libraries(memory_utils_test PRIVATE
    GTest::gtest
    GTest::gtest_main
)

hpc_set_compiler_options(memory_utils_test)
hpc_enable_sanitizers(memory_utils_test)

gtest_add_tests(TARGET memory_utils_test)
