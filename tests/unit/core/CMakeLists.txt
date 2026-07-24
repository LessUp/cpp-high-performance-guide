# Core module tests CMakeLists.txt

add_executable(core_test
    core_test.cpp
)

target_link_libraries(core_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    hpc_headers
)

hpc_set_compiler_options(core_test)
hpc_enable_sanitizers(core_test)

gtest_add_tests(TARGET core_test)
