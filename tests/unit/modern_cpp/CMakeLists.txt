add_executable(modern_cpp_examples_test
    modern_cpp_examples_test.cpp
)

target_include_directories(modern_cpp_examples_test PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/03-modern-cpp/include
)

target_link_libraries(modern_cpp_examples_test PRIVATE
    GTest::gtest
    GTest::gtest_main
)

hpc_set_compiler_options(modern_cpp_examples_test)
hpc_enable_sanitizers(modern_cpp_examples_test)

gtest_add_tests(TARGET modern_cpp_examples_test)

add_executable(modern_cpp_header_only_smoke_test
    modern_cpp_header_only_smoke_test.cpp
    modern_cpp_header_only_smoke_extra.cpp
)

target_include_directories(modern_cpp_header_only_smoke_test PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/03-modern-cpp/include
)

hpc_set_compiler_options(modern_cpp_header_only_smoke_test)
hpc_enable_sanitizers(modern_cpp_header_only_smoke_test)

add_test(NAME modern_cpp_header_only_smoke_test COMMAND modern_cpp_header_only_smoke_test)
