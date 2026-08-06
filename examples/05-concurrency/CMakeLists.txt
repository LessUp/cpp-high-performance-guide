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

# Thread affinity example (Linux-only API, pthread_setaffinity_np)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    hpc_add_example(
        NAME thread_affinity
        SOURCES src/thread_affinity.cpp
        LIBRARIES Threads::Threads
    )
endif()

# NUMA binding example (requires libnuma).
# /usr/include cannot be added directly: CMake strips it as a compiler-
# implicit path under the conda sysroot mapping, and forcing it would shadow
# the toolchain's own headers. Instead a shim directory exposes just the
# numa headers; their remaining includes resolve from the normal sysroot.
if(HPC_NUMA_LIBRARY AND HPC_NUMA_INCLUDE_DIR)
    set(_hpc_numa_shim "${CMAKE_BINARY_DIR}/numa-headers")
    file(MAKE_DIRECTORY "${_hpc_numa_shim}")
    foreach(_hpc_numa_header numa.h numaif.h numacompat1.h)
        if(EXISTS "${HPC_NUMA_INCLUDE_DIR}/${_hpc_numa_header}")
            execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${HPC_NUMA_INCLUDE_DIR}/${_hpc_numa_header}"
                "${_hpc_numa_shim}/${_hpc_numa_header}")
        endif()
    endforeach()

    # Real compile+link probe: distro libnuma builds may require glibc
    # symbols newer than a cross/sysroot toolchain provides (e.g. conda gcc
    # vs system libnuma). Skip the example instead of failing at link time.
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES "${_hpc_numa_shim}")
    set(CMAKE_REQUIRED_LIBRARIES "${HPC_NUMA_LIBRARY}")
    check_cxx_source_compiles("
        #include <numa.h>
        int main() { return numa_available() >= 0 ? 0 : 1; }
    " HPC_NUMA_LINKS)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)

    if(HPC_NUMA_LINKS)
        hpc_add_example(
            NAME numa_binding
            SOURCES src/numa_binding.cpp
            INCLUDE_DIRS ${_hpc_numa_shim}
            LIBRARIES ${HPC_NUMA_LIBRARY} Threads::Threads
        )
    else()
        message(STATUS "numa_binding skipped: libnuma does not link with this toolchain")
    endif()
endif()
