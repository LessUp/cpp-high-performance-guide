# 03 - Modern C++ Performance Features

<p align="center">
  <img src="https://img.shields.io/badge/Standard-C%2B%2B20-blue.svg" alt="C++20">
  <img src="https://img.shields.io/badge/Difficulty-Beginner-green.svg" alt="Difficulty">
  <img src="https://img.shields.io/badge/Topic-Language%20Features-purple.svg" alt="Topic">
</p>

> Leverage C++20 features for zero-cost abstractions and significant performance gains.

Discover how `constexpr`, move semantics, container optimizations, and C++20 Ranges can deliver **2-1000x speedups**.

---

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

---

## Common Pitfalls

### ❌ Forgetting std::move

```cpp
// BAD: Unnecessary copy
Buffer process(Buffer input) {
    Buffer result = input;  // Copy!
    return result;
}

// GOOD: Move semantics
Buffer process(Buffer input) {
    return input;  // Implicit move
}
```

### ❌ Not reserving vector capacity

```cpp
// BAD: Multiple reallocations
std::vector<int> v;
for (int i = 0; i < 10000; ++i) {
    v.push_back(i);  // May reallocate many times
}

// GOOD: Single allocation
std::vector<int> v;
v.reserve(10000);
for (int i = 0; i < 10000; ++i) {
    v.push_back(i);  // No reallocation
}
```

### ❌ Using consteval incorrectly

```cpp
// BAD: Runtime value can't be used with consteval
int runtime_value = get_input();
consteval_func(runtime_value);  // Error!

// GOOD: Use constexpr for runtime-compatible compile-time
constexpr_func(runtime_value);  // Works
```

---

## Knowledge Check

Test your understanding:

1. **When should you use `consteval` instead of `constexpr`?**
   <details>
   <summary>Click for answer</summary>
   Use `consteval` when you want to guarantee compile-time evaluation and prevent any possibility of runtime execution. Useful for compile-time constants, template parameters, or ensuring no runtime overhead.
   </details>

2. **What makes move semantics faster than copy semantics?**
   <details>
   <summary>Click for answer</summary>
   Move semantics transfer ownership of resources (like heap-allocated memory) instead of duplicating them. A move is typically O(1) (pointer swap), while a copy is O(n) (deep copy of all data).
   </details>

3. **Does `reserve()` change the size of a vector?**
   <details>
   <summary>Click for answer</summary>
   No, `reserve()` only affects capacity, not size. It pre-allocates memory to avoid reallocations during subsequent `push_back()` calls. The vector remains empty (size = 0) after `reserve()`.
   </details>

---

## Further Reading

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)

---

## Next Steps

- Continue to [SIMD Vectorization](../04-simd-vectorization/) to learn about CPU vector units
- Practice with [Modern C++ Exercises](../../docs/zh/exercises/module-03-modern-cpp.md)
