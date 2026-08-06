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
    BENCHMARK_SOURCES bench/alignment_bench.cpp bench/scalar_baseline.cpp
    ENABLE_SIMD AVX2
)

# The alignment benchmark's scalar baseline must stay genuinely scalar:
# disable auto-vectorization for its translation unit only, so the comparison
# against AVX is not flattened by an auto-vectorized "scalar" reference.
if(TARGET alignment_bench)
    if(HPC_IS_CLANG)
        set_source_files_properties(bench/scalar_baseline.cpp PROPERTIES
            COMPILE_OPTIONS "-fno-vectorize;-fno-slp-vectorize")
    elseif(HPC_IS_GCC)
        set_source_files_properties(bench/scalar_baseline.cpp PROPERTIES
            COMPILE_OPTIONS "-fno-tree-vectorize")
    endif()
endif()

# Prefetch example
hpc_add_example(
    NAME prefetch
    SOURCES src/prefetch.cpp
    BENCHMARK_SOURCES bench/prefetch_bench.cpp
)

# Lookup structure cache-friendliness comparison
hpc_add_example(
    NAME data_structures
    SOURCES src/data_structures.cpp
    BENCHMARK_SOURCES bench/data_structures_bench.cpp
)
