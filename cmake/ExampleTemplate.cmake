# ExampleTemplate.cmake
# Provides standardized functions for creating example modules
# Ensures consistent structure across all examples

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
# Creates an example executable with optional benchmark
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
    
    # Create the example executable
    add_executable(${ARG_NAME} ${ARG_SOURCES})
    
    # Set compiler options
    hpc_set_compiler_options(${ARG_NAME})
    
    # Enable sanitizers if configured
    hpc_enable_sanitizers(${ARG_NAME})
    
    # Add include directories (target-based, not directory-based!)
    if(ARG_INCLUDE_DIRS)
        target_include_directories(${ARG_NAME} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()
    
    # Link libraries
    if(ARG_LIBRARIES)
        target_link_libraries(${ARG_NAME} PRIVATE ${ARG_LIBRARIES})
    endif()
    
    # Enable OpenMP if requested
    if(ARG_ENABLE_OPENMP AND OpenMP_CXX_FOUND)
        target_link_libraries(${ARG_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif()
    
    # Enable SIMD if requested
    if(ARG_ENABLE_SIMD)
        foreach(simd_level ${ARG_ENABLE_SIMD})
            hpc_enable_simd(${ARG_NAME} ${simd_level})
        endforeach()
    endif()
    
    # Create benchmark if sources provided
    if(ARG_BENCHMARK_SOURCES AND HPC_BUILD_BENCHMARKS)
        set(bench_name "${ARG_NAME}_bench")
        add_executable(${bench_name} ${ARG_BENCHMARK_SOURCES})
        
        hpc_set_compiler_options(${bench_name})
        
        target_link_libraries(${bench_name} PRIVATE
            benchmark::benchmark
            benchmark::benchmark_main
        )
        
        if(ARG_INCLUDE_DIRS)
            target_include_directories(${bench_name} PRIVATE ${ARG_INCLUDE_DIRS})
        endif()
        
        if(ARG_LIBRARIES)
            target_link_libraries(${bench_name} PRIVATE ${ARG_LIBRARIES})
        endif()
        
        if(ARG_ENABLE_OPENMP AND OpenMP_CXX_FOUND)
            target_link_libraries(${bench_name} PRIVATE OpenMP::OpenMP_CXX)
        endif()
        
        if(ARG_ENABLE_SIMD)
            foreach(simd_level ${ARG_ENABLE_SIMD})
                hpc_enable_simd(${bench_name} ${simd_level})
            endforeach()
        endif()
        
        # Add to run_all_benchmarks target
        add_dependencies(run_all_benchmarks ${bench_name})
        
        # Register benchmark as a test (optional, for CI)
        add_test(NAME ${bench_name} COMMAND ${bench_name} --benchmark_min_time=0.1)
    endif()
    
    message(STATUS "Added example: ${ARG_NAME}")
endfunction()

#------------------------------------------------------------------------------
# hpc_add_header_only_library(
#     NAME <name>
#     INCLUDE_DIR <include directory>
# )
#
# Creates a header-only interface library
#------------------------------------------------------------------------------
function(hpc_add_header_only_library)
    cmake_parse_arguments(
        ARG
        ""
        "NAME;INCLUDE_DIR"
        ""
        ${ARGN}
    )
    
    if(NOT ARG_NAME)
        message(FATAL_ERROR "hpc_add_header_only_library: NAME is required")
    endif()
    
    if(NOT ARG_INCLUDE_DIR)
        message(FATAL_ERROR "hpc_add_header_only_library: INCLUDE_DIR is required")
    endif()
    
    add_library(${ARG_NAME} INTERFACE)
    target_include_directories(${ARG_NAME} INTERFACE ${ARG_INCLUDE_DIR})
    
    message(STATUS "Added header-only library: ${ARG_NAME}")
endfunction()

#------------------------------------------------------------------------------
# hpc_add_benchmark(
#     NAME <name>
#     SOURCES <source files...>
#     [INCLUDE_DIRS <include directories...>]
#     [LIBRARIES <libraries to link...>]
# )
#
# Creates a standalone benchmark executable
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
