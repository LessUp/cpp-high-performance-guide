# SIMD Wrapper API

C++ wrapper for SIMD intrinsics providing a clean, portable interface for vectorized operations.

---

## Overview

**Header:** `examples/04-simd-vectorization/include/simd_wrapper.hpp`

**Namespace:** `hpc::simd`

---

## SIMD Level Detection

The library automatically detects available SIMD instruction sets at compile time:

| Macro | Instruction Set | Width |
|-------|-----------------|-------|
| `HPC_HAS_SSE2` | SSE2 | 128-bit (4 floats) |
| `HPC_HAS_AVX` | AVX | 256-bit (8 floats) |
| `HPC_HAS_AVX2` | AVX2 | 256-bit (8 floats) |
| `HPC_HAS_AVX512` | AVX-512 | 512-bit (16 floats) |

---

## SimdVec Class

### Template Parameters

```cpp
template<typename T, size_t Width>
class SimdVec;
```

- `T` - Element type (currently `float` is specialized)
- `Width` - Number of elements (4, 8, or 16)

---

### Common Interface

All SIMD vector types share this interface:

#### Construction

```cpp
// Default constructor - zero initialized
SimdVec();

// Broadcast a single value to all lanes
explicit SimdVec(float val);

// Load from unaligned memory
SimdVec(const float* ptr);

// Load from aligned memory (static method)
static SimdVec load_aligned(const float* ptr);
```

#### Storage

```cpp
// Store to unaligned memory
void store(float* ptr) const;

// Store to aligned memory
void store_aligned(float* ptr) const;
```

#### Element Access

```cpp
// Get element at index (slow, for debugging)
float operator[](size_t i) const;
```

#### Arithmetic Operators

```cpp
SimdVec operator+(const SimdVec& other) const;
SimdVec operator-(const SimdVec& other) const;
SimdVec operator*(const SimdVec& other) const;
SimdVec operator/(const SimdVec& other) const;

SimdVec& operator+=(const SimdVec& other);
SimdVec& operator-=(const SimdVec& other);
SimdVec& operator*=(const SimdVec& other);
```

#### Mathematical Operations

```cpp
// Sum all lanes into a single value
float horizontal_sum() const;

// Fused multiply-add: a * b + c
static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c);

// Element-wise square root
SimdVec sqrt() const;

// Element-wise minimum
SimdVec min(const SimdVec& other) const;

// Element-wise maximum
SimdVec max(const SimdVec& other) const;
```

---

## Type Aliases

### FloatVec

```cpp
using FloatVec = SimdVec<float, WIDTH>;  // WIDTH depends on available SIMD
```

Default SIMD vector type, automatically selects the widest available instruction set.

| Available SIMD | FloatVec Width |
|----------------|----------------|
| AVX-512 | 16 floats |
| AVX2 | 8 floats |
| SSE2 | 4 floats |
| None | 4 floats (scalar fallback) |

### FLOAT_VEC_WIDTH

```cpp
constexpr size_t FLOAT_VEC_WIDTH;  // 4, 8, or 16
```

Number of floats in the default `FloatVec` type.

---

## High-Level Operations

### add_arrays_wrapped

```cpp
void add_arrays_wrapped(const float* a, const float* b, float* c, size_t n);
```

Add two arrays element-wise: `c[i] = a[i] + b[i]`

**Example:**
```cpp
float a[1024], b[1024], c[1024];
// ... initialize a and b ...

hpc::simd::add_arrays_wrapped(a, b, c, 1024);
```

---

### dot_product_wrapped

```cpp
float dot_product_wrapped(const float* a, const float* b, size_t n);
```

Compute dot product: `sum(a[i] * b[i])`

**Example:**
```cpp
float a[1024], b[1024];
// ... initialize ...

float result = hpc::simd::dot_product_wrapped(a, b, 1024);
```

---

### scale_array_wrapped

```cpp
void scale_array_wrapped(float* arr, float scalar, size_t n);
```

Scale array by scalar: `arr[i] *= scalar`

---

### clamp_array_wrapped

```cpp
void clamp_array_wrapped(float* arr, float min_val, float max_val, size_t n);
```

Clamp array values to range: `arr[i] = clamp(arr[i], min_val, max_val)`

---

## Usage Examples

### Basic Vector Operations

```cpp
#include "simd_wrapper.hpp"

using namespace hpc::simd;

void process_arrays(float* a, float* b, float* result, size_t n) {
    size_t i = 0;
    
    // Process in SIMD-width chunks
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(&a[i]);
        FloatVec vb(&b[i]);
        
        // result = a * 2 + b
        FloatVec scaled = va * FloatVec(2.0f);
        FloatVec vr = scaled + vb;
        
        vr.store(&result[i]);
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        result[i] = a[i] * 2.0f + b[i];
    }
}
```

### Using Fused Multiply-Add

```cpp
float compute_weighted_sum(const float* values, const float* weights, 
                           float bias, size_t n) {
    FloatVec sum(bias);
    size_t i = 0;
    
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec v(&values[i]);
        FloatVec w(&weights[i]);
        sum = FloatVec::fmadd(v, w, sum);  // sum += v * w
    }
    
    float result = sum.horizontal_sum();
    
    // Handle remainder
    for (; i < n; ++i) {
        result += values[i] * weights[i];
    }
    
    return result;
}
```

### Aligned Memory for Best Performance

```cpp
#include "memory_utils.hpp"
#include "simd_wrapper.hpp"

void aligned_operations() {
    // Allocate aligned memory
    auto a = hpc::memory::make_aligned<float>(1024, 64);
    auto b = hpc::memory::make_aligned<float>(1024, 64);
    auto c = hpc::memory::make_aligned<float>(1024, 64);
    
    // ... initialize ...
    
    size_t i = 0;
    for (; i + FLOAT_VEC_WIDTH <= 1024; i += FLOAT_VEC_WIDTH) {
        // Use aligned loads for better performance
        FloatVec va = FloatVec::load_aligned(&a[i]);
        FloatVec vb = FloatVec::load_aligned(&b[i]);
        FloatVec vc = va + vb;
        vc.store_aligned(&c[i]);
    }
}
```

---

## Performance Considerations

### Alignment

For best performance:
- Use `load_aligned()` and `store_aligned()` when data is 64-byte aligned
- Aligned loads avoid extra instructions on some architectures

### Remainder Handling

Always handle elements that don't fit in a full SIMD vector:

```cpp
size_t i = 0;
for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
    // SIMD loop
}
for (; i < n; ++i) {
    // Scalar remainder
}
```

### Memory Bandwidth

SIMD is most beneficial when:
- Data is in cache (memory-bound operations won't benefit as much)
- Operations are compute-intensive
- Data access is sequential

---

## Quick Reference

| Operation | Method | SIMD Equivalent |
|-----------|--------|-----------------|
| Add | `a + b` | `_mm_add_ps` |
| Subtract | `a - b` | `_mm_sub_ps` |
| Multiply | `a * b` | `_mm_mul_ps` |
| Divide | `a / b` | `_mm_div_ps` |
| FMA | `fmadd(a,b,c)` | `_mm_fmadd_ps` |
| Sqrt | `a.sqrt()` | `_mm_sqrt_ps` |
| Horizontal sum | `a.horizontal_sum()` | Manual reduction |
| Min | `a.min(b)` | `_mm_min_ps` |
| Max | `a.max(b)` | `_mm_max_ps` |

---

## See Also

- [SIMD Module Examples](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/04-simd-vectorization)
- [Memory Utilities API](memory-utils.md) - For aligned allocation
- [Optimization Decision Tree](../../guides/optimization-decision-tree.md)
