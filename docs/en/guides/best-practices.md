# Best Practices

Industry-tested patterns and recommendations for high-performance C++ programming.

---

## Table of Contents

1. [Code Organization](#code-organization)
2. [Memory Management](#memory-management)
3. [Cache Optimization](#cache-optimization)
4. [SIMD & Vectorization](#simd--vectorization)
5. [Concurrency](#concurrency)
6. [Benchmarking](#benchmarking)

---

## Code Organization

### Namespace Organization

Use a project-specific namespace to avoid conflicts:

```cpp
namespace hpc {
    namespace memory {
        // Memory-related utilities
    }
    namespace simd {
        // SIMD operations
    }
    namespace concurrent {
        // Concurrency primitives
    }
}
```

### Header Guards

Use `#pragma once` (modern) or traditional include guards:

```cpp
#pragma once

// OR

#ifndef HPC_MODULE_FILENAME_HPP
#define HPC_MODULE_FILENAME_HPP
// ...
#endif // HPC_MODULE_FILENAME_HPP
```

### CMake Structure

- One `CMakeLists.txt` per module
- Use `target_` commands (not global commands)
- Declare dependencies explicitly

---

## Memory Management

### Prefer Stack Allocation

```cpp
// Good: Stack allocation
void process() {
    std::array<int, 100> buffer;  // Fast, cache-friendly
}

// Avoid: Unnecessary heap allocation
void process() {
    auto buffer = std::make_unique<int[]>(100);  // Slower
}
```

### Use Smart Pointers

```cpp
// Ownership semantics
std::unique_ptr<Resource> owner;      // Exclusive ownership
std::shared_ptr<Resource> shared;     // Shared ownership
std::weak_ptr<Resource> weak_ref;     // Non-owning reference
```

### Alignment for SIMD

```cpp
// Align data for SIMD operations
alignas(64) float buffer[256];  // 64-byte aligned for AVX-512

// Or use aligned allocation
auto* ptr = static_cast<float*>(
    std::aligned_alloc(64, size * sizeof(float))
);
```

---

## Cache Optimization

### Data Structure Layout

```cpp
// SOA (Structure of Arrays) - Better for SIMD/cache
struct ParticleSystem {
    std::vector<float> pos_x, pos_y, pos_z;
    std::vector<float> vel_x, vel_y, vel_z;
    std::vector<float> mass;
};

// AOS (Array of Structures) - Better for OOP/access patterns
struct Particle {
    float pos[3], vel[3], mass;
};
std::vector<Particle> particles;
```

### Access Pattern

```cpp
// Good: Sequential access
for (size_t i = 0; i < n; ++i) {
    data[i] = compute(i);
}

// Bad: Random access
for (size_t i = 0; i < n; ++i) {
    data[random_idx[i]] = compute(i);
}
```

### False Sharing Prevention

```cpp
// Pad to cache line size to prevent false sharing
struct alignas(64) Counter {
    std::atomic<size_t> value{0};
    char padding[64 - sizeof(std::atomic<size_t>)];
};

std::array<Counter, num_threads> counters;
```

---

## SIMD & Vectorization

### Compiler Hints

```cpp
// Enable auto-vectorization
void process(float* __restrict a, 
             const float* __restrict b, 
             size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] += b[i];  // Compiler can vectorize this
    }
}
```

### Explicit SIMD

```cpp
#include <immintrin.h>

void add_avx2(float* dst, const float* a, const float* b, size_t n) {
    size_t i = 0;
    // Process 8 floats at a time
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&dst[i], vc);
    }
    // Handle remainder
    for (; i < n; ++i) {
        dst[i] = a[i] + b[i];
    }
}
```

---

## Concurrency

### Thread Safety

```cpp
// Thread-safe initialization
std::once_flag init_flag;
void initialize() {
    std::call_once(init_flag, []() {
        // One-time initialization
    });
}

// Atomic operations
std::atomic<size_t> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);
```

### Memory Ordering

```cpp
// seq_cst: strongest, default
atomic.store(value);  // seq_cst

// Release/Acquire: for synchronization
atomic.store(value, std::memory_order_release);
auto val = atomic.load(std::memory_order_acquire);

// Relaxed: for counters only
counter.fetch_add(1, std::memory_order_relaxed);
```

### Lock-Free Data Structures

```cpp
// Single-producer, single-consumer queue
template<typename T, size_t Size>
class SPSCQueue {
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    alignas(64) std::array<T, Size> buffer_;
    // ...
};
```

---

## Benchmarking

### Google Benchmark Best Practices

```cpp
#include <benchmark/benchmark.h>

// Prevent optimization
static void BM_vector_pushback(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v.data());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_vector_pushback)->Range(8, 8<<10);

// Statistical significance
BENCHMARK(BM_example)
    ->Repetitions(10)
    ->ReportAggregatesOnly(true);
```

### Environment Control

```bash
# Disable CPU frequency scaling
sudo cpupower frequency-set --governor performance

# Pin to specific CPU
taskset -c 0 ./benchmark

# Disable ASLR for reproducibility
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

---

## References

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Optimizing Software in C++](https://agner.org/optimize/optimizing_cpp.pdf)
- [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)
