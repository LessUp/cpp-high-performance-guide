# SIMD Exercises

Practice auto-vectorization, intrinsics, and SIMD wrapper usage.

---

## Exercise 1: Enable Auto-Vectorization

### Objective

Write code that the compiler can automatically vectorize.

### Setup

```cpp
// File: auto_vectorize_exercise.cpp
#include <vector>
#include <cstddef>

// TODO: Make this vectorizable
void add_arrays(float* a, const float* b, const float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}
```

### Tasks

1. **Compile with vectorization report**
   ```bash
   g++ -O3 -march=native -fopt-info-vec-optimized auto_vectorize_exercise.cpp
   ```

2. **Check if vectorized**

3. **If not, modify the code** to enable vectorization

### Questions

<details>
<summary>Q: What prevents auto-vectorization?</summary>

Common blockers:
- Pointer aliasing (compiler can't prove a, b, c don't overlap)
- Loop-carried dependencies
- Unknown trip count
- Complex control flow
- Function calls inside loop
</details>

### Hint

<details>
<summary>Click for hint</summary>

Add `__restrict` to tell the compiler pointers don't overlap:

```cpp
void add_arrays(float* __restrict a, const float* __restrict b, 
                const float* __restrict c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}
```
</details>

---

## Exercise 2: Convert Scalar to SIMD

### Objective

Convert a scalar function to use AVX2 intrinsics.

### Setup

```cpp
// Scalar version
void scale_array_scalar(float* arr, float scalar, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

// TODO: Implement AVX2 version
void scale_array_avx2(float* arr, float scalar, size_t n) {
    // Your code here
}
```

### Tasks

1. **Implement the AVX2 version** using `_mm256_*` intrinsics

2. **Handle the remainder** (elements not divisible by 8)

3. **Benchmark both versions**
   ```bash
   g++ -O3 -march=native -mavx2 scale_exercise.cpp -o scale_exercise
   ./scale_exercise
   ```

### Reference

```cpp
#include <immintrin.h>

// Load 8 floats (unaligned)
__m256 _mm256_loadu_ps(const float* addr);

// Store 8 floats (unaligned)
void _mm256_storeu_ps(float* addr, __m256 a);

// Broadcast scalar to all lanes
__m256 _mm256_set1_ps(float a);

// Multiply
__m256 _mm256_mul_ps(__m256 a, __m256 b);
```

### Solution Skeleton

<details>
<summary>Click for solution skeleton</summary>

```cpp
void scale_array_avx2(float* arr, float scalar, size_t n) {
    __m256 vscalar = _mm256_set1_ps(scalar);
    
    size_t i = 0;
    // Process 8 floats at a time
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        v = _mm256_mul_ps(v, vscalar);
        _mm256_storeu_ps(&arr[i], v);
    }
    
    // Handle remainder
    for (; i < n; ++i) {
        arr[i] *= scalar;
    }
}
```
</details>

---

## Exercise 3: SIMD Wrapper Usage

### Objective

Use the provided SIMD wrapper for cleaner code.

### Tasks

1. **Rewrite Exercise 2** using `hpc::simd::FloatVec`

2. **Compare readability**

3. **Benchmark both implementations**

### Reference

```cpp
#include "simd_wrapper.hpp"

using namespace hpc::simd;

FloatVec v(&arr[i]);           // Load
FloatVec vscalar(scalar);       // Broadcast
FloatVec result = v * vscalar;  // Multiply
result.store(&arr[i]);          // Store
```

---

## Exercise 4: Horizontal Operations

### Objective

Implement a dot product using SIMD.

### Setup

```cpp
// Scalar version
float dot_product_scalar(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// TODO: Implement SIMD version
float dot_product_simd(const float* a, const float* b, size_t n) {
    // Your code here
}
```

### Hint

<details>
<summary>Click for hint</summary>

Use FMA (fused multiply-add) for efficiency:
1. Accumulate products in a SIMD vector
2. Use `horizontal_sum()` at the end
3. Handle remainder elements separately
</details>

---

## Exercise 5: Comparison Operations

### Objective

Implement clamping using SIMD min/max operations.

### Tasks

1. **Implement `clamp_array`** that limits values to [min_val, max_val]

2. **Use SIMD min/max intrinsics**

3. **Benchmark against scalar**

### Reference

```cpp
__m256 _mm256_min_ps(__m256 a, __m256 b);
__m256 _mm256_max_ps(__m256 a, __m256 b);
```

---

## Challenge: Optimize a Real Algorithm

### Objective

Optimize a Mandelbrot set calculation using SIMD.

### Setup

The Mandelbrot calculation is embarrassingly parallel - each pixel is independent.

```cpp
int mandelbrot(float cx, float cy, int max_iter) {
    float x = 0, y = 0;
    int iter = 0;
    while (x*x + y*y <= 4.0f && iter < max_iter) {
        float temp = x*x - y*y + cx;
        y = 2*x*y + cy;
        x = temp;
        iter++;
    }
    return iter;
}
```

### Tasks

1. **Vectorize** to process 8 pixels simultaneously

2. **Handle the control flow** (early exit on divergence)

3. **Measure speedup**

### Hint

<details>
<summary>Click for hint</summary>

Since SIMD doesn't support early exit per lane, use a mask to track which pixels have diverged:

```cpp
__m256 cx_vec, cy_vec;  // 8 c values
__m256 x_vec = _mm256_setzero_ps();
__m256 y_vec = _mm256_setzero_ps();
__m256 iter_vec = _mm256_setzero_ps();
__m256 active_mask = _mm256_castsi256_ps(_mm256_set1_epi32(-1));

for (int iter = 0; iter < max_iter; ++iter) {
    // Only process active pixels
    // ...
}
```
</details>

---

## Solutions

See [solutions.md](solutions.md) for complete solutions.

---

## Next Steps

- Continue to [Concurrency Exercises](module-05-concurrency.md)
- Read the [SIMD Wrapper API](../reference/api/simd-wrapper.md)
- Explore [Optimization Decision Tree](../guides/optimization-decision-tree.md)
