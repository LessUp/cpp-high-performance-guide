# Examples CMakeLists.txt
# Each subdirectory contains a self-contained optimization example module

option(HPC_BUILD_EXAMPLE_CMAKE_MODERN "Build 01-cmake-modern examples" ON)
option(HPC_BUILD_EXAMPLE_MEMORY_CACHE "Build 02-memory-cache examples" ON)
option(HPC_BUILD_EXAMPLE_MODERN_CPP "Build 03-modern-cpp examples" ON)
option(HPC_BUILD_EXAMPLE_SIMD_VECTORIZATION "Build 04-simd-vectorization examples" ON)
option(HPC_BUILD_EXAMPLE_CONCURRENCY "Build 05-concurrency examples" ON)
option(HPC_BUILD_EXAMPLE_DISTRIBUTED_MPI "Build 06-distributed-mpi examples (requires HPC_ENABLE_MPI=ON)" ON)
option(HPC_BUILD_EXAMPLE_IO_PERFORMANCE "Build 07-io-performance examples (Linux only)" ON)
option(HPC_BUILD_EXAMPLE_CPU_MICROARCH "Build 08-cpu-microarch examples" ON)

if(HPC_BUILD_EXAMPLE_CMAKE_MODERN)
    add_subdirectory(01-cmake-modern)
endif()

if(HPC_BUILD_EXAMPLE_MEMORY_CACHE)
    add_subdirectory(02-memory-cache)
endif()

if(HPC_BUILD_EXAMPLE_MODERN_CPP)
    add_subdirectory(03-modern-cpp)
endif()

if(HPC_BUILD_EXAMPLE_SIMD_VECTORIZATION)
    add_subdirectory(04-simd-vectorization)
endif()

if(HPC_BUILD_EXAMPLE_CONCURRENCY)
    add_subdirectory(05-concurrency)
endif()

# MPI examples need both the module switch and a discovered MPI runtime.
# When MPI is absent the module is skipped silently, keeping the default
# build (and CI) dependency-free.
if(HPC_BUILD_EXAMPLE_DISTRIBUTED_MPI AND MPI_CXX_FOUND)
    add_subdirectory(06-distributed-mpi)
endif()

if(HPC_BUILD_EXAMPLE_IO_PERFORMANCE)
    add_subdirectory(07-io-performance)
endif()

if(HPC_BUILD_EXAMPLE_CPU_MICROARCH)
    add_subdirectory(08-cpu-microarch)
endif()
