# Modern C++ Performance Features

This module demonstrates how modern C++ features can improve performance.

## Contents

| File | Topic | Key Concept |
|------|-------|-------------|
| `src/compile_time.cpp` | Compile-Time Computation | constexpr/consteval |
| `src/move_semantics.cpp` | Move Semantics | Avoid copies |
| `src/vector_reserve.cpp` | Container Optimization | Reduce allocations |
| `src/ranges_vs_loops.cpp` | C++20 Ranges | Modern iteration |

## Key Concepts

### Compile-Time Computation

Move computation from runtime to compile time:

```cpp
// Runtime computation
int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// Compile-time computation
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// Guaranteed compile-time (C++20)
consteval int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}
```

### Move Semantics

Avoid expensive deep copies:

```cpp
class Buffer {
    std::unique_ptr<char[]> data_;
    size_t size_;
public:
    // Move constructor - O(1)
    Buffer(Buffer&& other) noexcept
        : data_(std::move(other.data_)), size_(other.size_) {
        other.size_ = 0;
    }
};

// Use std::move to enable moving
Buffer b1(1024);
Buffer b2 = std::move(b1);  // Move, not copy
```

### Vector Reserve

Prevent reallocations:

```cpp
// Bad: multiple reallocations
std::vector<int> v;
for (int i = 0; i < 10000; ++i) {
    v.push_back(i);  // May reallocate
}

// Good: single allocation
std::vector<int> v;
v.reserve(10000);
for (int i = 0; i < 10000; ++i) {
    v.push_back(i);  // No reallocation
}
```

### C++20 Ranges

Modern, composable iteration:

```cpp
// Traditional
std::vector<int> result;
for (int x : data) {
    if (x % 2 == 0) {
        result.push_back(x * x);
    }
}

// C++20 Ranges
auto result = data 
    | std::views::filter([](int x) { return x % 2 == 0; })
    | std::views::transform([](int x) { return x * x; });
```

## Running Benchmarks

```bash
cmake --preset=release
cmake --build build/release

# Run benchmarks
./build/release/examples/03-modern-cpp/bench/compile_time_bench
./build/release/examples/03-modern-cpp/bench/move_semantics_bench
./build/release/examples/03-modern-cpp/bench/vector_reserve_bench
./build/release/examples/03-modern-cpp/bench/ranges_bench
```

## Expected Results

| Benchmark | Expected Speedup |
|-----------|------------------|
| constexpr vs runtime | Near-zero runtime |
| Move vs Copy | 10-1000x (depends on data size) |
| Reserve vs No Reserve | 2-5x |
| Ranges vs Loops | ~1x (similar performance) |

## Further Reading

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
