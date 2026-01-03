# SIMD and Vectorization Examples

# Create a header-only library for SIMD utilities
add_library(simd_utils INTERFACE)
target_include_directories(simd_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Auto-vectorization example
hpc_add_example(
    NAME auto_vectorize
    SOURCES src/auto_vectorize.cpp
    LIBRARIES simd_utils
    ENABLE_SIMD AVX2
)

# SIMD intrinsics introduction
hpc_add_example(
    NAME intrinsics_intro
    SOURCES src/intrinsics_intro.cpp
    LIBRARIES simd_utils
    ENABLE_SIMD AVX2
)

# SIMD benchmark
add_executable(simd_bench bench/simd_bench.cpp)
target_include_directories(simd_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(simd_bench PRIVATE benchmark::benchmark simd_utils)
hpc_set_compiler_options(simd_bench)

# Enable SIMD for benchmark
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(simd_bench PRIVATE -mavx2 -mfma)
endif()
