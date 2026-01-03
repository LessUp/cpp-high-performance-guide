cmake_minimum_required(VERSION 3.20)

project(hpc-optimization-guide
    VERSION 1.0.0
    DESCRIPTION "High Performance Computing Optimization Guide - Modern C++ Examples"
    LANGUAGES CXX
)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include CMake modules
include(Dependencies)
include(CompilerOptions)
include(Sanitizers)
include(ExampleTemplate)

# Options
option(HPC_BUILD_TESTS "Build tests" ON)
option(HPC_BUILD_BENCHMARKS "Build benchmarks" ON)
option(HPC_ENABLE_OPENMP "Enable OpenMP support" ON)

# Find OpenMP if enabled
if(HPC_ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
    else()
        message(WARNING "OpenMP not found, some examples will be disabled")
    endif()
endif()

# Find Threads for concurrency tests
find_package(Threads REQUIRED)

# Fetch dependencies
hpc_fetch_google_benchmark()
if(HPC_BUILD_TESTS)
    hpc_fetch_google_test()
    hpc_fetch_rapidcheck()
endif()

# Add subdirectories
add_subdirectory(benchmarks)
add_subdirectory(examples)

if(HPC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== HPC Optimization Guide Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Tests: ${HPC_BUILD_TESTS}")
message(STATUS "Build Benchmarks: ${HPC_BUILD_BENCHMARKS}")
message(STATUS "OpenMP: ${HPC_ENABLE_OPENMP}")
message(STATUS "=============================================")
message(STATUS "")
