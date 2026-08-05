# Benchmarks CMakeLists.txt
# Aggregated benchmark target. Shared benchmark utilities live in
# common/benchmark_utils.hpp (consumed via a direct include directory).

# Aggregate all benchmarks into a single build target
# (cmake --build build/release --target run_all_benchmarks)
add_custom_target(run_all_benchmarks
    COMMENT "Building all benchmarks..."
)
