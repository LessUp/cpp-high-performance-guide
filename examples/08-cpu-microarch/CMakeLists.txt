# CPU microarchitecture examples: branch prediction, ILP, PMU counters.

# The scalar kernels must stay genuinely scalar: they demonstrate
# branch-prediction and dependency-latency effects that the auto-vectorizer
# would erase. Disable vectorization for that translation unit only (same
# pattern as 04-simd-vectorization/bench/scalar_baseline.cpp).
# -fno-if-conversion matters as much as -fno-vectorize here: GCC would
# otherwise lower the data-dependent if into cmov and the misprediction
# effect (the whole point of the demo) silently disappears.
if(HPC_IS_CLANG)
    set_source_files_properties(src/microarch_kernels.cpp PROPERTIES
        COMPILE_OPTIONS "-fno-vectorize;-fno-slp-vectorize;-fno-if-conversion")
elseif(HPC_IS_GCC)
    set_source_files_properties(src/microarch_kernels.cpp PROPERTIES
        COMPILE_OPTIONS "-fno-tree-vectorize;-fno-if-conversion")
endif()

hpc_add_example(
    NAME branch_prediction
    SOURCES src/branch_prediction.cpp src/microarch_kernels.cpp
    BENCHMARK_SOURCES bench/microarch_bench.cpp src/microarch_kernels.cpp
)

hpc_add_example(
    NAME dependency_chains
    SOURCES src/dependency_chains.cpp src/microarch_kernels.cpp
)

hpc_add_example(
    NAME pmu_counters
    SOURCES src/pmu_counters.cpp
)
