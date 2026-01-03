# Modern C++ Features Optimization Examples

# Compile-time computation example
hpc_add_example(
    NAME compile_time
    SOURCES src/compile_time.cpp
)

# Move semantics example
hpc_add_example(
    NAME move_semantics
    SOURCES src/move_semantics.cpp
    BENCHMARK_SOURCES bench/move_semantics_bench.cpp
)

# Vector reserve example
hpc_add_example(
    NAME vector_reserve
    SOURCES src/vector_reserve.cpp
    BENCHMARK_SOURCES bench/vector_reserve_bench.cpp
)

# Ranges vs loops example
hpc_add_example(
    NAME ranges_vs_loops
    SOURCES src/ranges_vs_loops.cpp
    BENCHMARK_SOURCES bench/ranges_bench.cpp
)
