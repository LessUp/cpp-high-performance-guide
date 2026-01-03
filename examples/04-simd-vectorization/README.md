# SIMD Vectorization

This module demonstrates SIMD (Single Instruction, Multiple Data) programming techniques.

## Contents

| File | Topic | Key Concept |
|------|-------|-------------|
| `src/auto_vectorize.cpp` | Auto-Vectorization | Compiler-friendly patterns |
| `src/intrinsics_intro.cpp` | SIMD Intrinsics | Manual SSE/AVX/AVX-512 |
| `include/simd_wrapper.hpp` | SIMD Wrapper | Readable abstractions |

## Key Concepts

### Auto-Vectorization

Write code that compilers can automatically vectorize:

```cpp
// Good: simple loop, no dependencies
void add_arrays(float* a, const float* b, const float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}

// Bad: loop-carried dependency
void prefix_sum(float* a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        a[i] += a[i-1];  // Depends on previous iteration
    }
}
```

Check vectorization with compiler flags:
```bash
# GCC
g++ -O3 -march=native -fopt-info-vec-optimized file.cpp

# Clang
clang++ -O3 -march=native -Rpass=loop-vectorize file.cpp
```

### SIMD Intrinsics

Manual vectorization for maximum control:

```cpp
#include <immintrin.h>

// SSE: 4 floats at once
void add_sse(float* a, const float* b, const float* c, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        __m128 vb = _mm_load_ps(&b[i]);
        __m128 vc = _mm_load_ps(&c[i]);
        __m128 va = _mm_add_ps(vb, vc);
        _mm_store_ps(&a[i], va);
    }
}

// AVX2: 8 floats at once
void add_avx2(float* a, const float* b, const float* c, size_t n) {
    for (size_t i = 0; i < n; i += 8) {
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_load_ps(&c[i]);
        __m256 va = _mm256_add_ps(vb, vc);
        _mm256_store_ps(&a[i], va);
    }
}
```

### SIMD Wrapper

Use the provided wrapper for readable SIMD code:

```cpp
#include "simd_wrapper.hpp"

using Vec = simd::Vec256<float>;

void add_wrapped(float* a, const float* b, const float* c, size_t n) {
    for (size_t i = 0; i < n; i += Vec::size()) {
        Vec vb = Vec::load(&b[i]);
        Vec vc = Vec::load(&c[i]);
        Vec va = vb + vc;
        va.store(&a[i]);
    }
}
```

## Instruction Sets

| ISA | Register Width | Floats/Op | Doubles/Op |
|-----|----------------|-----------|------------|
| SSE | 128-bit | 4 | 2 |
| AVX2 | 256-bit | 8 | 4 |
| AVX-512 | 512-bit | 16 | 8 |

## Running Benchmarks

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/04-simd-vectorization/bench/simd_bench
```

## Expected Results

| Implementation | Relative Speed |
|----------------|----------------|
| Scalar | 1x |
| Auto-vectorized | 2-4x |
| SSE | 3-4x |
| AVX2 | 6-8x |
| AVX-512 | 10-16x |

Actual speedup depends on:
- CPU architecture
- Memory bandwidth
- Data alignment
- Operation complexity

## CPU Feature Detection

Check your CPU capabilities:
```bash
# Linux
cat /proc/cpuinfo | grep flags

# Look for: sse, sse2, sse4_1, avx, avx2, avx512f
```

## Further Reading

- [Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/)
- [Agner Fog's Optimization Manuals](https://www.agner.org/optimize/)
