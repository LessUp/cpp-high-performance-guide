# C++ High Performance Computing Optimization Guide

[![Build](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/build.yml/badge.svg)](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/build.yml)
[![Benchmarks](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/benchmark.yml/badge.svg)](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/benchmark.yml)
[![Sanitizers](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/YOUR_USERNAME/cpp-high-performance-guide/actions/workflows/sanitizers.yml)

A comprehensive collection of high-performance computing optimization examples and best practices for modern C++20.

## Features

- **Modern CMake Build System** - Target-based CMake with presets, FetchContent dependencies
- **Memory & Cache Optimization** - AOS vs SOA, false sharing, alignment, prefetching
- **Modern C++ Features** - constexpr, move semantics, vector reserve, C++20 ranges
- **SIMD Vectorization** - Auto-vectorization, SSE/AVX2/AVX-512 intrinsics, wrapper library
- **Concurrency** - Atomic operations, lock-free queues, OpenMP integration
- **Benchmarking Framework** - Google Benchmark integration, FlameGraph generation

## Quick Start

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 14+)
- CMake 3.20+
- Ninja (recommended) or Make

### Build

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/cpp-high-performance-guide.git
cd cpp-high-performance-guide

# Configure and build (Release mode)
cmake --preset=release
cmake --build build/release

# Run all benchmarks
cd build/release && ctest --output-on-failure
```

### Available Presets

| Preset | Description |
|--------|-------------|
| `debug` | Debug build with symbols |
| `release` | Optimized release build (-O3, -march=native) |
| `asan` | AddressSanitizer enabled |
| `tsan` | ThreadSanitizer enabled |

```bash
# Build with sanitizers
cmake --preset=asan
cmake --build build/asan
```

## Project Structure

```
cpp-high-performance-guide/
├── cmake/                      # CMake modules
│   ├── CompilerOptions.cmake   # Compiler flags management
│   ├── Dependencies.cmake      # FetchContent dependencies
│   ├── Sanitizers.cmake        # Sanitizer configuration
│   └── ExampleTemplate.cmake   # Example module template
├── examples/
│   ├── 01-cmake-modern/        # CMake best practices vs anti-patterns
│   ├── 02-memory-cache/        # Memory and cache optimization
│   ├── 03-modern-cpp/          # Modern C++ features
│   ├── 04-simd-vectorization/  # SIMD and vectorization
│   └── 05-concurrency/         # Concurrent programming
├── benchmarks/                 # Benchmark utilities
├── tests/                      # Unit and property tests
├── tools/                      # Analysis and profiling tools
└── docs/                       # Documentation
```

## Example Modules

### 01 - Modern CMake
Learn CMake best practices through anti-pattern vs best-practice comparisons.
- Target-based vs directory-based commands
- FetchContent dependency management
- Compiler options management

### 02 - Memory & Cache Optimization
Master cache-friendly programming techniques.
- **AOS vs SOA**: Data layout impact on performance
- **False Sharing**: Multi-threaded cache line contention
- **Alignment**: SIMD-friendly memory alignment
- **Prefetching**: Manual prefetch hints

### 03 - Modern C++ Features
Leverage modern C++ for performance.
- **constexpr/consteval**: Compile-time computation
- **Move Semantics**: Avoid unnecessary copies
- **Vector Reserve**: Reduce allocations
- **C++20 Ranges**: Modern iteration patterns

### 04 - SIMD Vectorization
Maximize CPU throughput with SIMD.
- **Auto-vectorization**: Compiler-friendly patterns
- **Intrinsics**: SSE, AVX2, AVX-512 examples
- **SIMD Wrapper**: Readable SIMD abstractions

### 05 - Concurrency
Write efficient multi-threaded code.
- **Atomic Operations**: Memory ordering explained
- **Lock-Free Queue**: SPSC queue implementation
- **OpenMP**: Simple parallelization patterns

## Running Benchmarks

```bash
# Run all benchmarks
cd build/release
ctest --output-on-failure

# Run specific benchmark
./examples/02-memory-cache/bench/aos_soa_bench

# Export benchmark results to JSON
./examples/02-memory-cache/bench/aos_soa_bench --benchmark_format=json > results.json
```

## Profiling

Generate FlameGraph visualizations:

```bash
# Record performance data
./tools/flamegraph/generate_flamegraph.sh ./build/release/examples/02-memory-cache/bench/aos_soa_bench

# View the generated SVG
firefox flamegraph.svg
```

## Documentation

- [Learning Path](docs/learning-path.md) - Recommended order for studying examples
- [Profiling Guide](docs/profiling-guide.md) - How to profile and analyze performance

## Contributing

1. Fork the repository
2. Create a feature branch
3. Ensure all tests pass with sanitizers
4. Submit a pull request

## License

MIT License - see [LICENSE](LICENSE) for details.

## Acknowledgments

- [Google Benchmark](https://github.com/google/benchmark)
- [Google Test](https://github.com/google/googletest)
- [RapidCheck](https://github.com/emil-e/rapidcheck)
- [FlameGraph](https://github.com/brendangregg/FlameGraph)
