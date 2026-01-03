# Memory and Cache Optimization Examples

# Header-only library for memory utilities
hpc_add_header_only_library(
    NAME memory_utils
    INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# AOS vs SOA example
hpc_add_example(
    NAME aos_vs_soa
    SOURCES src/aos_vs_soa.cpp
    BENCHMARK_SOURCES bench/aos_soa_bench.cpp
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# False sharing example
hpc_add_example(
    NAME false_sharing
    SOURCES src/false_sharing.cpp
    BENCHMARK_SOURCES bench/false_sharing_bench.cpp
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    ENABLE_OPENMP
)

# Memory alignment example
hpc_add_example(
    NAME alignment
    SOURCES src/alignment.cpp
    BENCHMARK_SOURCES bench/alignment_bench.cpp
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    ENABLE_SIMD AVX2
)

# Prefetch example
hpc_add_example(
    NAME prefetch
    SOURCES src/prefetch.cpp
    BENCHMARK_SOURCES bench/prefetch_bench.cpp
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
