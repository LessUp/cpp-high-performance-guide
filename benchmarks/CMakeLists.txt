# Benchmarks CMakeLists.txt
# Common benchmark utilities and aggregated benchmark targets

# Add common benchmark utilities as interface library
add_library(benchmark_common INTERFACE)
target_include_directories(benchmark_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_link_libraries(benchmark_common INTERFACE
    benchmark::benchmark
    benchmark::benchmark_main
)

# Aggregate all benchmarks into a single target
add_custom_target(run_all_benchmarks
    COMMENT "Running all benchmarks..."
)
