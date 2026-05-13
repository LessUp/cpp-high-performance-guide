# Core module tests CMakeLists.txt

add_executable(core_test
    core_test.cpp
)

target_include_directories(core_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(core_test PRIVATE
    GTest::gtest
    GTest::gtest_main
)

hpc_set_compiler_options(core_test)
hpc_enable_sanitizers(core_test)

gtest_add_tests(TARGET core_test)
