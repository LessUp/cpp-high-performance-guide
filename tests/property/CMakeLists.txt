# Property-based tests CMakeLists.txt
# Using RapidCheck for property-based testing

# Memory properties test
add_executable(memory_properties_test
    memory_properties.cpp
)

target_include_directories(memory_properties_test PRIVATE
    ${CMAKE_SOURCE_DIR}/benchmarks/common
)

target_link_libraries(memory_properties_test PRIVATE
    GTest::gtest
    GTest::gtest_main
    rapidcheck
    rapidcheck_gtest
    benchmark::benchmark
    hpc_headers
)

hpc_set_compiler_options(memory_properties_test)
hpc_enable_sanitizers(memory_properties_test)

gtest_add_tests(TARGET memory_properties_test)

# SIMD properties test
add_executable(simd_properties_test
    simd_properties.cpp
)

target_link_libraries(simd_properties_test PRIVATE
    GTest::gtest
    rapidcheck
    rapidcheck_gtest
    benchmark::benchmark
    hpc_headers
)

hpc_set_compiler_options(simd_properties_test)
hpc_enable_sanitizers(simd_properties_test)

# Enable SIMD for the test (only on x86/x64, not ARM)
if(NOT HPC_IS_ARM)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(simd_properties_test PRIVATE -mavx2 -mfma)
    elseif(MSVC)
        target_compile_options(simd_properties_test PRIVATE /arch:AVX2)
    endif()
endif()

gtest_add_tests(TARGET simd_properties_test)

# Concurrency properties test
add_executable(concurrency_properties_test
    concurrency_properties.cpp
)

target_link_libraries(concurrency_properties_test PRIVATE
    GTest::gtest
    rapidcheck
    rapidcheck_gtest
    Threads::Threads
    hpc_headers
)

# Link OpenMP if available
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(concurrency_properties_test PRIVATE OpenMP::OpenMP_CXX)
endif()

hpc_set_compiler_options(concurrency_properties_test)
hpc_enable_sanitizers(concurrency_properties_test)

gtest_add_tests(TARGET concurrency_properties_test)

# Benchmark properties test
add_executable(benchmark_properties_test
    benchmark_properties.cpp
)

target_include_directories(benchmark_properties_test PRIVATE
    ${CMAKE_SOURCE_DIR}/benchmarks/common
)

target_link_libraries(benchmark_properties_test PRIVATE
    GTest::gtest
    benchmark::benchmark
)

hpc_set_compiler_options(benchmark_properties_test)
hpc_enable_sanitizers(benchmark_properties_test)

gtest_add_tests(TARGET benchmark_properties_test)
