# I/O performance examples (Linux/POSIX: read/pread/mmap/write paths)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    hpc_add_example(
        NAME mmap_vs_read
        SOURCES src/mmap_vs_read.cpp
    )

    hpc_add_example(
        NAME buffered_write
        SOURCES src/buffered_write.cpp
    )

    hpc_add_benchmark(
        NAME io_bench
        SOURCES bench/io_bench.cpp
    )
else()
    message(STATUS "Skipping 07-io-performance: module uses Linux/POSIX I/O APIs")
endif()
