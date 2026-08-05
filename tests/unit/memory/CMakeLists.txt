# Memory unit tests

add_executable(memory_utils_test
    memory_utils_test.cpp
)

target_link_libraries(memory_utils_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    hpc_headers
)

hpc_set_compiler_options(memory_utils_test)
hpc_enable_sanitizers(memory_utils_test)

gtest_add_tests(TARGET memory_utils_test)

add_executable(particle_types_test
    particle_types_test.cpp
)

target_link_libraries(particle_types_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    hpc_headers
)

hpc_set_compiler_options(particle_types_test)
hpc_enable_sanitizers(particle_types_test)

gtest_add_tests(TARGET particle_types_test)
