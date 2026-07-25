# Concurrency and Multi-threading Examples

# Atomic ordering example
hpc_add_example(
    NAME atomic_ordering
    SOURCES src/atomic_ordering.cpp
    BENCHMARK_SOURCES bench/atomic_bench.cpp
    LIBRARIES Threads::Threads
)

# Lock-free queue example
hpc_add_example(
    NAME lock_free_queue
    SOURCES src/lock_free_queue.cpp
    LIBRARIES Threads::Threads
)

# OpenMP basics example
if(OpenMP_CXX_FOUND)
    hpc_add_example(
        NAME openmp_basics
        SOURCES src/openmp_basics.cpp
        BENCHMARK_SOURCES bench/openmp_bench.cpp
        ENABLE_OPENMP
    )
endif()
