# SIMD unit tests

add_executable(simd_utils_test
    simd_utils_test.cpp
)

target_include_directories(simd_utils_test PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/04-simd-vectorization/include
)

target_link_libraries(simd_utils_test PRIVATE
    GTest::gtest
    GTest::gtest_main
)

hpc_set_compiler_options(simd_utils_test)
hpc_enable_sanitizers(simd_utils_test)

# Enable SIMD for the test (only on x86/x64, not ARM)
if(NOT HPC_IS_ARM)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(simd_utils_test PRIVATE -mavx2 -mfma)
    elseif(MSVC)
        target_compile_options(simd_utils_test PRIVATE /arch:AVX2)
    endif()
endif()

gtest_add_tests(TARGET simd_utils_test)
