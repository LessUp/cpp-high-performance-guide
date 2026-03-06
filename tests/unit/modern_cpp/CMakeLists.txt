add_executable(modern_cpp_examples_test
    modern_cpp_examples_test.cpp
)

target_link_libraries(modern_cpp_examples_test PRIVATE
    GTest::gtest
    GTest::gtest_main
)

hpc_set_compiler_options(modern_cpp_examples_test)
hpc_enable_sanitizers(modern_cpp_examples_test)

gtest_discover_tests(modern_cpp_examples_test)
