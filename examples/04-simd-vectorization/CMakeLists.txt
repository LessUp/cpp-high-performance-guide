# SIMD and Vectorization Examples

# Dispatch example - uses canonical hpc::simd module
hpc_add_example(
    NAME dispatch_example
    SOURCES src/dispatch_example_main.cpp
)

# Auto-vectorization example
hpc_add_example(
    NAME auto_vectorize
    SOURCES src/auto_vectorize.cpp
    ENABLE_OPENMP
    ENABLE_SIMD AVX2
)

# SIMD intrinsics introduction
hpc_add_example(
    NAME intrinsics_intro
    SOURCES src/intrinsics_intro.cpp
    ENABLE_SIMD AVX2
)

# SIMD benchmark
hpc_add_benchmark(
    NAME simd_bench
    SOURCES bench/simd_bench.cpp
)

# Enable SIMD for benchmark (only on x86/x64 and only when the benchmark target exists)
if(TARGET simd_bench AND NOT HPC_IS_ARM)
    hpc_enable_simd(simd_bench AVX2)
endif()
