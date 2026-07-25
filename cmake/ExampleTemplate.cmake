# ExampleTemplate.cmake
# Provides standardized functions for creating example modules
# Ensures consistent structure across all examples

#------------------------------------------------------------------------------
# _hpc_configure_target(target
#     [ENABLE_OPENMP]
#     [INCLUDE_DIRS <dirs...>]
#     [LIBRARIES <libs...>]
#     [ENABLE_SIMD <SSE|AVX|AVX2|AVX512>...]
# )
#
# Internal helper: applies the configuration shared by an example executable
# and its benchmark counterpart (compiler options, sanitizers, canonical
# headers, include dirs, link libs, OpenMP, SIMD). Centralizing this keeps the
# demo and benchmark targets in lockstep so a new wiring change only needs to
# be made once.
#------------------------------------------------------------------------------
function(_hpc_configure_target target)
    cmake_parse_arguments(
        ARG
        "ENABLE_OPENMP"
        ""
        "INCLUDE_DIRS;LIBRARIES;ENABLE_SIMD"
        ${ARGN}
    )

    hpc_set_compiler_options(${target})
    hpc_enable_sanitizers(${target})
    target_link_libraries(${target} PRIVATE hpc_headers)

    if(ARG_INCLUDE_DIRS)
        target_include_directories(${target} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()

    if(ARG_LIBRARIES)
        target_link_libraries(${target} PRIVATE ${ARG_LIBRARIES})
    endif()

    if(ARG_ENABLE_OPENMP AND OpenMP_CXX_FOUND)
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif()

    # hpc_enable_simd handles the ARM/x86/MSVC split internally, so no
    # HPC_IS_ARM guard is needed here. This keeps demo and benchmark targets
    # consistent (previously only the demo path guarded on HPC_IS_ARM).
    if(ARG_ENABLE_SIMD)
        foreach(simd_level ${ARG_ENABLE_SIMD})
            hpc_enable_simd(${target} ${simd_level})
        endforeach()
    endif()
endfunction()

#------------------------------------------------------------------------------
# hpc_add_example(
#     NAME <name>
#     SOURCES <source files...>
#     [BENCHMARK_SOURCES <benchmark source files...>]
#     [INCLUDE_DIRS <include directories...>]
#     [LIBRARIES <libraries to link...>]
#     [ENABLE_OPENMP]
#     [ENABLE_SIMD <SSE|AVX|AVX2|AVX512>]
# )
#
# Creates an example executable with an optional benchmark. The benchmark is
# registered as a CTest test carrying the "benchmark" label so CI can exclude
# it (ctest -LE benchmark) and run only correctness tests.
#------------------------------------------------------------------------------
function(hpc_add_example)
    cmake_parse_arguments(
        ARG
        "ENABLE_OPENMP"
        "NAME"
        "SOURCES;BENCHMARK_SOURCES;INCLUDE_DIRS;LIBRARIES;ENABLE_SIMD"
        ${ARGN}
    )

    if(NOT ARG_NAME)
        message(FATAL_ERROR "hpc_add_example: NAME is required")
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "hpc_add_example: SOURCES is required")
    endif()

    # Forward ENABLE_OPENMP to _hpc_configure_target. CMake has no shell-style
    # ${VAR:+word} expansion, so build the flag list explicitly.
    set(openmp_arg "")
    if(ARG_ENABLE_OPENMP)
        set(openmp_arg ENABLE_OPENMP)
    endif()

    # Create the example executable
    add_executable(${ARG_NAME} ${ARG_SOURCES})
    _hpc_configure_target(${ARG_NAME}
        INCLUDE_DIRS ${ARG_INCLUDE_DIRS}
        LIBRARIES ${ARG_LIBRARIES}
        ${openmp_arg}
        ENABLE_SIMD ${ARG_ENABLE_SIMD}
    )

    # Create benchmark if sources provided
    if(ARG_BENCHMARK_SOURCES AND HPC_BUILD_BENCHMARKS)
        set(bench_name "${ARG_NAME}_bench")
        add_executable(${bench_name} ${ARG_BENCHMARK_SOURCES})
        _hpc_configure_target(${bench_name}
            INCLUDE_DIRS ${ARG_INCLUDE_DIRS}
            LIBRARIES ${ARG_LIBRARIES}
            ${openmp_arg}
            ENABLE_SIMD ${ARG_ENABLE_SIMD}
        )
        target_link_libraries(${bench_name} PRIVATE
            benchmark::benchmark
            benchmark::benchmark_main
        )
        add_dependencies(run_all_benchmarks ${bench_name})
        add_test(NAME ${bench_name} COMMAND ${bench_name} --benchmark_min_time=0.1)
        set_tests_properties(${bench_name} PROPERTIES LABELS "benchmark")
    endif()

    message(STATUS "Added example: ${ARG_NAME}")
endfunction()

#------------------------------------------------------------------------------
# hpc_add_benchmark(
#     NAME <name>
#     SOURCES <source files...>
#     [INCLUDE_DIRS <include directories...>]
#     [LIBRARIES <libraries to link...>]
# )
#
# Creates a standalone benchmark executable. Deliberately does NOT apply
# sanitizers (they distort timing) and does not accept ENABLE_OPENMP/ENABLE_SIMD
# (use hpc_add_example with BENCHMARK_SOURCES if those are needed).
#------------------------------------------------------------------------------
function(hpc_add_benchmark)
    cmake_parse_arguments(
        ARG
        ""
        "NAME"
        "SOURCES;INCLUDE_DIRS;LIBRARIES"
        ${ARGN}
    )

    if(NOT ARG_NAME)
        message(FATAL_ERROR "hpc_add_benchmark: NAME is required")
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "hpc_add_benchmark: SOURCES is required")
    endif()

    if(NOT HPC_BUILD_BENCHMARKS)
        return()
    endif()

    add_executable(${ARG_NAME} ${ARG_SOURCES})

    hpc_set_compiler_options(${ARG_NAME})

    target_link_libraries(${ARG_NAME} PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        hpc_headers
    )

    if(ARG_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()

    if(ARG_LIBRARIES)
        target_link_libraries(${ARG_NAME} PRIVATE ${ARG_LIBRARIES})
    endif()

    add_dependencies(run_all_benchmarks ${ARG_NAME})

    message(STATUS "Added benchmark: ${ARG_NAME}")
endfunction()
