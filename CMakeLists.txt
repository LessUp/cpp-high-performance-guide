cmake_minimum_required(VERSION 3.20)

project(hpc-optimization-guide
    VERSION 1.3.0
    DESCRIPTION "High Performance Computing Optimization Guide - Modern C++ Examples"
    LANGUAGES CXX
)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Canonical header-only library (include/hpc/)
add_library(hpc_headers INTERFACE)
target_include_directories(hpc_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

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
option(HPC_ENABLE_MPI "Enable MPI support (distributed-memory examples)" OFF)
option(HPC_VECTORIZE_REPORT "Enable compiler vectorization reports (noisy)" OFF)

# Find OpenMP if enabled
if(HPC_ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
    else()
        message(WARNING "OpenMP not found, some examples will be disabled")
    endif()
endif()

# Optional: libnuma backs the NUMA binding example in 05-concurrency.
# PATHS hints: conda-distributed CMake/toolchain lacks the Debian multiarch
# lib dir and /usr/include in its default search lists; harmless on stock
# toolchains.
find_library(HPC_NUMA_LIBRARY numa PATHS /usr/lib/x86_64-linux-gnu /usr/lib64)
find_path(HPC_NUMA_INCLUDE_DIR numa.h PATHS /usr/include)
if(HPC_NUMA_LIBRARY AND HPC_NUMA_INCLUDE_DIR)
    message(STATUS "libnuma found: ${HPC_NUMA_LIBRARY}")
endif()

# Optional: Intel TBB backs std::execution::par/par_unseq on GCC's
# libstdc++. Without it the parallel STL example still builds and covers
# seq/unseq; par policies are compiled in only when TBB is present.
find_package(TBB QUIET)
if(TBB_FOUND)
    message(STATUS "TBB found: parallel STL example includes par/par_unseq")
endif()

# Find MPI if enabled. MPI is a heavyweight external dependency (separate
# compiler wrappers / runtime), so it stays opt-in: with the default OFF the
# whole tree configures and builds without any MPI installation.
if(HPC_ENABLE_MPI)
    find_package(MPI COMPONENTS CXX)
    if(MPI_CXX_FOUND)
        message(STATUS "MPI found: ${MPI_CXX_COMPILER}")
    else()
        message(WARNING
            "HPC_ENABLE_MPI=ON but no MPI implementation was found; "
            "06-distributed-mpi examples will be skipped")
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

# Enable testing before add_subdirectory() so that add_test() calls issued
# inside examples/ (benchmark tests via hpc_add_example) are registered.
# enable_testing() must precede every add_test().
enable_testing()

# Add subdirectories
add_subdirectory(benchmarks)
add_subdirectory(examples)

if(HPC_BUILD_TESTS)
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
message(STATUS "MPI: ${HPC_ENABLE_MPI}")
message(STATUS "LTO: ${HPC_ENABLE_LTO}")
message(STATUS "TBB: ${TBB_FOUND}")
message(STATUS "=============================================")
message(STATUS "")
