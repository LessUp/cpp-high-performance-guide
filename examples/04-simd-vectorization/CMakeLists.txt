# SIMD and Vectorization Examples

# Create a header-only library for SIMD utilities
add_library(simd_utils INTERFACE)
target_include_directories(simd_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Auto-vectorization example
hpc_add_example(
    NAME auto_vectorize
    SOURCES src/auto_vectorize.cpp
    LIBRARIES simd_utils
    ENABLE_SIMD AVX2
)

# SIMD intrinsics introduction
hpc_add_example(
    NAME intrinsics_intro
    SOURCES src/intrinsics_intro.cpp
    LIBRARIES simd_utils
    ENABLE_SIMD AVX2
)

# SIMD benchmark
hpc_add_benchmark(
    NAME simd_bench
    SOURCES bench/simd_bench.cpp
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    LIBRARIES simd_utils
)

# Enable SIMD for benchmark (only on x86/x64 and only when the benchmark target exists)
if(TARGET simd_bench AND NOT HPC_IS_ARM)
    hpc_enable_simd(simd_bench AVX2)
endif()
