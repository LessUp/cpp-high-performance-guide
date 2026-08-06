# GEMM stage cross-correctness tests

add_executable(gemm_test
    gemm_test.cpp
)

target_link_libraries(gemm_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    hpc_headers
    OpenMP::OpenMP_CXX
)

hpc_set_compiler_options(gemm_test ENABLE_OPENMP)
hpc_enable_sanitizers(gemm_test)
hpc_enable_simd(gemm_test AVX2)

gtest_add_tests(TARGET gemm_test)
