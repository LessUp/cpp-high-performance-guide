# Modern C++ Features Optimization Examples

# Move semantics example (with shared buffer.hpp)
hpc_add_example(
    NAME move_semantics
    SOURCES src/move_semantics.cpp
    BENCHMARK_SOURCES bench/move_semantics_bench.cpp
)

# Compile-time computation example
hpc_add_example(
    NAME compile_time
    SOURCES src/compile_time.cpp
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

# Parallel STL execution policies. seq/unseq always build; par/par_unseq
# require Intel TBB (GCC libstdc++ implements them on top of it).
if(TBB_FOUND)
    hpc_add_example(
        NAME parallel_stl
        SOURCES src/parallel_stl.cpp
        BENCHMARK_SOURCES bench/parallel_stl_bench.cpp
        LIBRARIES TBB::tbb
    )
    target_compile_definitions(parallel_stl PRIVATE HPC_HAS_TBB)
    target_compile_definitions(parallel_stl_bench PRIVATE HPC_HAS_TBB)
else()
    hpc_add_example(
        NAME parallel_stl
        SOURCES src/parallel_stl.cpp
        BENCHMARK_SOURCES bench/parallel_stl_bench.cpp
    )
endif()
