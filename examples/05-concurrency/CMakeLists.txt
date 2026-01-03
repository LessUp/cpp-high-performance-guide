# Concurrency and Multi-threading Examples

# Create include directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Atomic ordering example
hpc_add_example(
    NAME atomic_ordering
    SOURCES src/atomic_ordering.cpp
)
target_include_directories(atomic_ordering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Lock-free queue example
hpc_add_example(
    NAME lock_free_queue
    SOURCES src/lock_free_queue.cpp
)
target_include_directories(lock_free_queue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# OpenMP basics example
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    hpc_add_example(
        NAME openmp_basics
        SOURCES src/openmp_basics.cpp
    )
    target_include_directories(openmp_basics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(openmp_basics PRIVATE OpenMP::OpenMP_CXX)
endif()
