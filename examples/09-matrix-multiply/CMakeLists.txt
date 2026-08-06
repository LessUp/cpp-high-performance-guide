# GEMM case study: naive -> tiling -> SIMD -> OpenMP

# ENABLE_SIMD AVX2 also passes -mfma (see hpc_enable_simd), which the FMA
# micro-kernel in hpc/gemm.hpp uses when available. On ARM the same header
# falls through to the NEON path via -mcpu=native.
hpc_add_example(
    NAME matrix_multiply
    SOURCES src/matrix_multiply.cpp
    BENCHMARK_SOURCES bench/gemm_bench.cpp
    ENABLE_OPENMP
    ENABLE_SIMD AVX2
)
