# Microarchitecture kernel + PMU wrapper tests

add_executable(microarch_test
    microarch_test.cpp
    ../../../examples/08-cpu-microarch/src/microarch_kernels.cpp
)

target_link_libraries(microarch_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    hpc_headers
)

hpc_set_compiler_options(microarch_test)
hpc_enable_sanitizers(microarch_test)

gtest_add_tests(TARGET microarch_test)
