# Memory and Cache Optimization Examples

# AOS vs SOA example
hpc_add_example(
    NAME aos_vs_soa
    SOURCES src/aos_vs_soa.cpp
    BENCHMARK_SOURCES bench/aos_soa_bench.cpp
)

# False sharing example
hpc_add_example(
    NAME false_sharing
    SOURCES src/false_sharing.cpp
    BENCHMARK_SOURCES bench/false_sharing_bench.cpp
    ENABLE_OPENMP
)

# Memory alignment example
hpc_add_example(
    NAME alignment
    SOURCES src/alignment.cpp
    BENCHMARK_SOURCES bench/alignment_bench.cpp
    ENABLE_SIMD AVX2
)

# Prefetch example
hpc_add_example(
    NAME prefetch
    SOURCES src/prefetch.cpp
    BENCHMARK_SOURCES bench/prefetch_bench.cpp
)
