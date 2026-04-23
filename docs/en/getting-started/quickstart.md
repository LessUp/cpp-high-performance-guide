# Quick Start Guide

Get up and running with C++ High Performance Guide in under 5 minutes.

---

## Prerequisites

Before you begin, ensure you have:

- **Compiler**: GCC 11+, Clang 14+, or MSVC 2022+
- **CMake**: 3.20 or higher
- **Git**: For cloning the repository

Verify your compiler:
```bash
g++ --version  # Should show 11.0 or higher
clang++ --version  # Should show 14.0 or higher
```

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/LessUp/cpp-high-performance-guide.git
cd cpp-high-performance-guide
```

### 2. Configure the Project

Use CMake presets for reproducible builds:

```bash
# Release build (optimized for performance)
cmake --preset=release

# Or debug build (for development)
cmake --preset=debug
```

### 3. Build

```bash
cmake --build build/release
```

### 4. Run Tests

```bash
ctest --preset=release
```

### 5. Run a Benchmark

```bash
./build/release/examples/02-memory-cache/aos_soa_bench
```

---

## Available Build Presets

| Preset | Description | Use Case |
|--------|-------------|----------|
| `release` | -O3, march=native | Performance measurements |
| `debug` | -O0, -g | Development and debugging |
| `relwithdebinfo` | -O2, -g | Profiling with symbols |
| `asan` | AddressSanitizer | Memory error detection |
| `tsan` | ThreadSanitizer | Race condition detection |
| `ubsan` | UndefinedBehaviorSanitizer | UB detection |
| `coverage` | gcov | Code coverage analysis |

---

## Next Steps

1. **[Learning Path](../guides/learning-path.md)** - Follow the structured 6-week curriculum
2. **[First Example](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/01-cmake-modern)** - Explore Modern CMake
3. **[Profiling Guide](../guides/profiling-guide.md)** - Learn to measure performance

---

## Troubleshooting

### "CMake cannot find compiler"

Install a C++20 compatible compiler:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential g++-11

# Fedora
sudo dnf install gcc-c++

# macOS
brew install gcc
```

### "FetchContent failed"

Check network connection or use system packages:

```bash
# Ubuntu/Debian
sudo apt-get install libbenchmark-dev libgtest-dev
```

See [Troubleshooting Guide](../reference/troubleshooting.md) for more issues.
