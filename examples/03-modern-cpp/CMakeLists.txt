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
    # TBB headers reach the targets through a shim directory, mirroring the
    # numa pattern: TBB::tbb's /usr/include is silently dropped by CMake on
    # sysroot toolchains (conda) and would shadow the toolchain's own
    # headers if forced. Symlinking only the tbb/ and oneapi/ subtrees keeps
    # the rest of the include resolution untouched.
    get_target_property(_hpc_tbb_includes TBB::tbb INTERFACE_INCLUDE_DIRECTORIES)
    set(_hpc_tbb_shim "${CMAKE_BINARY_DIR}/tbb-headers")
    file(MAKE_DIRECTORY "${_hpc_tbb_shim}")
    foreach(_hpc_tbb_inc ${_hpc_tbb_includes})
        foreach(_hpc_tbb_sub tbb oneapi)
            if(EXISTS "${_hpc_tbb_inc}/${_hpc_tbb_sub}"
               AND NOT EXISTS "${_hpc_tbb_shim}/${_hpc_tbb_sub}")
                execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                    "${_hpc_tbb_inc}/${_hpc_tbb_sub}"
                    "${_hpc_tbb_shim}/${_hpc_tbb_sub}")
            endif()
        endforeach()
    endforeach()

    hpc_add_example(
        NAME parallel_stl
        SOURCES src/parallel_stl.cpp
        BENCHMARK_SOURCES bench/parallel_stl_bench.cpp
        INCLUDE_DIRS ${_hpc_tbb_shim}
        LIBRARIES TBB::tbb
    )
    target_compile_definitions(parallel_stl PRIVATE HPC_HAS_TBB)
    # The bench target only exists when HPC_BUILD_BENCHMARKS is ON (the
    # sanitizer presets disable it — sanitizers distort timing).
    if(TARGET parallel_stl_bench)
        target_compile_definitions(parallel_stl_bench PRIVATE HPC_HAS_TBB)
    endif()
else()
    hpc_add_example(
        NAME parallel_stl
        SOURCES src/parallel_stl.cpp
        BENCHMARK_SOURCES bench/parallel_stl_bench.cpp
    )
endif()

# Allocation strategies: per-object new/delete vs arena vs std::pmr
hpc_add_example(
    NAME allocators
    SOURCES src/allocators.cpp
    BENCHMARK_SOURCES bench/allocator_bench.cpp
)
