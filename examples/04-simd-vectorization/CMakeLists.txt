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
    SOURCES bench/simd_bench.cpp bench/scalar_baseline.cpp
)

if(TARGET simd_bench)
    # The scalar baseline must stay genuinely scalar: disable auto-vectorization
    # for its translation unit only. Without this, -O3 -march=native would
    # vectorize the "scalar" reference and invalidate the speedup comparison.
    if(HPC_IS_CLANG)
        set_source_files_properties(bench/scalar_baseline.cpp PROPERTIES
            COMPILE_OPTIONS "-fno-vectorize;-fno-slp-vectorize")
    elseif(HPC_IS_GCC)
        set_source_files_properties(bench/scalar_baseline.cpp PROPERTIES
            COMPILE_OPTIONS "-fno-tree-vectorize")
    endif()

    # Enable SIMD for benchmark (only on x86/x64)
    if(NOT HPC_IS_ARM)
        hpc_enable_simd(simd_bench AVX2)
    endif()
endif()
