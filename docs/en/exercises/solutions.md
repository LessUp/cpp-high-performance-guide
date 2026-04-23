# Exercise Solutions

Solutions and hints for the interactive exercises.

---

## How to Use This Page

1. **Attempt exercises first** - Don't read solutions until you've tried
2. **Use hints before solutions** - Hints guide without giving answers
3. **Understand, don't copy** - Make sure you understand *why* the solution works

---

## Memory Exercises Solutions

### Exercise 1: AOS to SOA Conversion

<details>
<summary>Solution</summary>

```cpp
// AOS version with mass field
struct ParticleAOS {
    float x, y, z;
    float vx, vy, vz;
    float mass;  // New field
};

// SOA version with mass field
struct ParticleSOA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;  // New field
};

// Update function for SOA (better for field-wise access)
void update_positions_soa(ParticleSOA& p, float dt, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        p.x[i] += p.vx[i] * dt;
        p.y[i] += p.vy[i] * dt;
        p.z[i] += p.vz[i] * dt;
    }
}
```

The SOA version maintains cache efficiency for field-wise operations even after adding the new field.
</details>

### Exercise 2: Fix False Sharing

<details>
<summary>Solution</summary>

```cpp
#include <atomic>
#include <thread>
#include <vector>

// Solution 1: Using alignas
struct alignas(64) AlignedCounter {
    std::atomic<int> value{0};
    // Padding is automatic due to alignas(64)
};

void increment_counters(AlignedCounter* counters, int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counters[thread_id].value.fetch_add(1, std::memory_order_relaxed);
    }
}

// Solution 2: Using CacheLinePadded from memory_utils.hpp
#include "memory_utils.hpp"

void increment_counters_padded(hpc::memory::CacheLinePadded<std::atomic<int>>* counters,
                               int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counters[thread_id].value.fetch_add(1, std::memory_order_relaxed);
    }
}
```

Expected speedup: 2-10x depending on thread count and CPU architecture.
</details>

---

## SIMD Exercises Solutions

### Exercise 1: Enable Auto-Vectorization

<details>
<summary>Solution</summary>

```cpp
// Add __restrict to tell compiler pointers don't overlap
void add_arrays(float* __restrict a, 
                const float* __restrict b, 
                const float* __restrict c, 
                size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}

// Alternative: Use restrict on each pointer separately
void add_arrays(float* restrict a, 
                const float* restrict b, 
                const float* restrict c, 
                size_t n);
```

Compile with: `g++ -O3 -march=native -fopt-info-vec-optimized` to verify vectorization.
</details>

### Exercise 2: Convert Scalar to SIMD

<details>
<summary>Solution</summary>

```cpp
#include <immintrin.h>

void scale_array_avx2(float* arr, float scalar, size_t n) {
    // Broadcast scalar to all 8 lanes
    __m256 vscalar = _mm256_set1_ps(scalar);
    
    size_t i = 0;
    
    // Process 8 floats at a time
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        v = _mm256_mul_ps(v, vscalar);
        _mm256_storeu_ps(&arr[i], v);
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        arr[i] *= scalar;
    }
}
```

For aligned arrays, use `_mm256_load_ps` and `_mm256_store_ps` for potentially better performance.
</details>

### Exercise 4: Dot Product

<details>
<summary>Solution</summary>

```cpp
#include <immintrin.h>

float dot_product_avx2(const float* a, const float* b, size_t n) {
    __m256 sum_vec = _mm256_setzero_ps();
    
    size_t i = 0;
    
    // Process 8 elements at a time
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // FMA: sum_vec = va * vb + sum_vec
        sum_vec = _mm256_fmadd_ps(va, vb, sum_vec);
    }
    
    // Horizontal sum of the 8 lanes
    // First, extract high and low 128-bit halves
    __m128 hi = _mm256_extractf128_ps(sum_vec, 1);
    __m128 lo = _mm256_castps256_ps128(sum_vec);
    __m128 sum128 = _mm_add_ps(hi, lo);
    
    // Shuffle and add
    __m128 shuf = _mm_shuffle_ps(sum128, sum128, _MM_SHUFFLE(2, 3, 0, 1));
    sum128 = _mm_add_ps(sum128, shuf);
    shuf = _mm_movehl_ps(shuf, sum128);
    sum128 = _mm_add_ss(sum128, shuf);
    
    float result = _mm_cvtss_f32(sum128);
    
    // Handle remaining elements
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}
```

Using SIMD wrapper (cleaner):

```cpp
#include "simd_wrapper.hpp"

float dot_product_wrapped(const float* a, const float* b, size_t n) {
    using namespace hpc::simd;
    
    FloatVec sum(0.0f);
    size_t i = 0;
    
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(&a[i]);
        FloatVec vb(&b[i]);
        sum = FloatVec::fmadd(va, vb, sum);
    }
    
    float result = sum.horizontal_sum();
    
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}
```
</details>

---

## Concurrency Exercises Solutions

### Exercise 1: Debug a Data Race

<details>
<summary>Solution</summary>

```cpp
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

// Solution: Use atomic
std::atomic<int> counter{0};

void increment(int times) {
    for (int i = 0; i < times; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    const int threads = 4;
    const int increments = 1000000;
    
    std::vector<std::thread> t;
    for (int i = 0; i < threads; ++i) {
        t.emplace_back(increment, increments);
    }
    
    for (auto& thread : t) {
        thread.join();
    }
    
    std::cout << "Expected: " << threads * increments << "\n";
    std::cout << "Actual: " << counter.load() << "\n";
    return 0;
}
```

Key fixes:
1. Changed `int` to `std::atomic<int>`
2. Used `fetch_add` instead of `++`
3. Used `memory_order_relaxed` since we only need atomicity, not ordering
</details>

### Exercise 3: Thread-Safe Counter

<details>
<summary>Solution</summary>

```cpp
#include <atomic>
#include <cstddef>

class ThreadSafeCounter {
public:
    void increment() {
        value_.fetch_add(1, std::memory_order_relaxed);
    }
    
    int get() const {
        return value_.load(std::memory_order_acquire);
    }
    
    void reset() {
        value_.store(0, std::memory_order_release);
    }
    
private:
    // alignas ensures no false sharing with adjacent data
    alignas(64) std::atomic<int> value_{0};
};

// For multiple counters that won't false-share:
struct CounterArray {
    alignas(64) std::atomic<int> counter0{0};
    alignas(64) std::atomic<int> counter1{0};
    alignas(64) std::atomic<int> counter2{0};
    alignas(64) std::atomic<int> counter3{0};
};
```
</details>

---

## Challenge Solutions

### Mandelbrot SIMD

<details>
<summary>Solution (partial)</summary>

```cpp
#include <immintrin.h>
#include <cmath>

void mandelbrot_avx2(float* cx, float* cy, int* result, 
                     int width, int height, int max_iter) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; x += 8) {
            __m256 cx_vec = _mm256_loadu_ps(&cx[y * width + x]);
            __m256 cy_vec = _mm256_loadu_ps(&cy[y * width + x]);
            
            __m256 x_vec = _mm256_setzero_ps();
            __m256 y_vec = _mm256_setzero_ps();
            __m256 iter_vec = _mm256_setzero_ps();
            __m256 one_vec = _mm256_set1_ps(1.0f);
            __m256 four_vec = _mm256_set1_ps(4.0f);
            
            for (int iter = 0; iter < max_iter; ++iter) {
                __m256 x2 = _mm256_mul_ps(x_vec, x_vec);
                __m256 y2 = _mm256_mul_ps(y_vec, y_vec);
                __m256 r2 = _mm256_add_ps(x2, y2);
                
                // Check if still bounded
                __m256 mask = _mm256_cmp_ps(r2, four_vec, _CMP_LE_OS);
                if (_mm256_testz_ps(mask, mask)) {
                    break;  // All pixels diverged
                }
                
                // Only update active pixels
                __m256 temp = _mm256_add_ps(
                    _mm256_sub_ps(x2, y2), cx_vec);
                y_vec = _mm256_add_ps(
                    _mm256_mul_ps(
                        _mm256_mul_ps(_mm256_set1_ps(2.0f), x_vec), 
                        y_vec), 
                    cy_vec);
                x_vec = temp;
                
                // Increment iteration count for active pixels
                iter_vec = _mm256_add_ps(iter_vec, 
                    _mm256_and_ps(one_vec, mask));
            }
            
            // Store results
            _mm256_storeu_ps(reinterpret_cast<float*>(&result[y * width + x]), 
                            iter_vec);
        }
    }
}
```

Note: This is a simplified version. A complete solution needs proper handling of the result storage and may need additional optimizations.
</details>

---

## Tips for Success

1. **Always measure** - Don't assume optimizations work
2. **Use sanitizers** - TSAN catches data races you might miss
3. **Check compiler output** - Verify vectorization happened
4. **Profile first** - Find bottlenecks before optimizing
5. **Understand the hardware** - Cache, memory, CPU architecture matter

---

## Need More Help?

- [FAQ](../reference/faq.md)
- [Troubleshooting](../reference/troubleshooting.md)
- [GitHub Discussions](https://github.com/LessUp/cpp-high-performance-guide/discussions)
