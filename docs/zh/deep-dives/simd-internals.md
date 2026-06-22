---
type: deep-dive
difficulty: advanced
prerequisites:
  - /zh/academy/module-atlas
  - /zh/architecture/repository-topology
description: 深入探讨 SIMD 向量化内部机制，涵盖自动向量化条件、intrinsic 封装、运行时分发机制。
---

# SIMD 内部机制深度专题

本深度专题探讨现代 C++ 应用中 SIMD（单指令多数据）向量化的内部机制。

## SIMD 基础

### 向量化的优势

SIMD 用一条指令处理多个数据元素：

```
Scalar: 4 operations for 4 floats
┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐
│ a+b │ │ c+d │ │ e+f │ │ g+h │
└─────┘ └─────┘ └─────┘ └─────┘

SIMD (SSE): 1 operation for 4 floats
┌─────────────────────┐
│ [a,c,e,g] + [b,d,f,h] │
└─────────────────────┘
```

### SIMD 寄存器大小

| ISA | 寄存器大小 | Float 数 | Double 数 | Int32 数 |
|-----|---------------|--------|---------|-------|
| SSE2 | 128 位 | 4 | 2 | 4 |
| AVX | 256 位 | 8 | 4 | 8 |
| AVX-512 | 512 位 | 16 | 8 | 16 |

## 自动向量化条件

当满足特定条件时，编译器可以自动对循环进行向量化。

### 良好模式

```cpp
// ✅ Simple, contiguous access
void add_arrays(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// ✅ No aliasing (restrict)
void add_arrays_restrict(float* __restrict a, float* __restrict b,
                         float* __restrict c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// ✅ Known trip count
constexpr size_t N = 1024;
void process_known(float data[N]) {
    for (size_t i = 0; i < N; ++i) {
        data[i] *= 2.0f;
    }
}
```

### 不良模式

```cpp
// ❌ Data dependency (prefix sum)
void prefix_sum(float* data, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        data[i] += data[i - 1];  // Each iteration depends on previous
    }
}

// ❌ Indirect access (gather)
void gather(float* src, int* indices, float* dst, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src[indices[i]];  // Non-contiguous access
    }
}

// ❌ Branch inside loop
void conditional(float* data, size_t n, float threshold) {
    for (size_t i = 0; i < n; ++i) {
        if (data[i] > threshold) {  // Branch prevents vectorization
            data[i] *= 2.0f;
        }
    }
}

// ❌ Function call
void with_call(float* data, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        data[i] = std::sqrt(data[i]);  // Unknown function
    }
}
```

### 编译器提示

```cpp
// GCC/Clang: Suggest vectorization
#pragma GCC ivdep
for (size_t i = 0; i < n; ++i) {
    c[i] = a[i] + b[i];
}

// OpenMP SIMD
#pragma omp simd
for (size_t i = 0; i < n; ++i) {
    c[i] = a[i] + b[i];
}

// C++ std::execution (parallel + vectorized)
#include <execution>
std::transform(std::execution::par_unseq,
               a, a + n, b, c,
               [](float x, float y) { return x + y; });
```

## SIMD 封装设计

### 分层抽象

```cpp
// hpc/simd.hpp - Layered SIMD abstraction

namespace hpc::simd {

// === Scalar Fallback ===
struct Scalar {
    static constexpr size_t width = 1;

    static inline float add(float a, float b) { return a + b; }
    static inline float mul(float a, float b) { return a * b; }
    // ...
};

// === SSE2 (128-bit) ===
#ifdef __SSE2__
#include <xmmintrin.h>
#include <emmintrin.h>

struct SSE2 {
    static constexpr size_t width = 4;

    using Vec = __m128;

    static inline Vec add(Vec a, Vec b) { return _mm_add_ps(a, b); }
    static inline Vec mul(Vec a, Vec b) { return _mm_mul_ps(a, b); }
    static inline Vec load(const float* p) { return _mm_loadu_ps(p); }
    static inline void store(float* p, Vec v) { _mm_storeu_ps(p, v); }
};
#endif

// === AVX (256-bit) ===
#ifdef __AVX__
#include <immintrin.h>

struct AVX {
    static constexpr size_t width = 8;

    using Vec = __m256;

    static inline Vec add(Vec a, Vec b) { return _mm256_add_ps(a, b); }
    static inline Vec mul(Vec a, Vec b) { return _mm256_mul_ps(a, b); }
    static inline Vec load(const float* p) { return _mm256_loadu_ps(p); }
    static inline void store(float* p, Vec v) { return _mm256_storeu_ps(p, v); }
};
#endif

// === AVX-512 (512-bit) ===
#ifdef __AVX512F__
struct AVX512 {
    static constexpr size_t width = 16;

    using Vec = __m512;

    static inline Vec add(Vec a, Vec b) { return _mm512_add_ps(a, b); }
    static inline Vec mul(Vec a, Vec b) { return _mm512_mul_ps(a, b); }
    static inline Vec load(const float* p) { return _mm512_loadu_ps(p); }
    static inline void store(float* p, Vec v) { return _mm512_storeu_ps(p, v); }
};
#endif

} // namespace hpc::simd
```

### 运算符重载

```cpp
// Natural syntax through operator overloading
namespace hpc::simd {

struct Vec4 {
    __m128 data;

    Vec4 operator+(Vec4 other) const {
        return Vec4{_mm_add_ps(data, other.data)};
    }

    Vec4 operator*(Vec4 other) const {
        return Vec4{_mm_mul_ps(data, other.data)};
    }

    static Vec4 load(const float* p) {
        return Vec4{_mm_loadu_ps(p)};
    }

    void store(float* p) const {
        _mm_storeu_ps(p, data);
    }
};

// Usage
void add_arrays(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        Vec4 va = Vec4::load(a + i);
        Vec4 vb = Vec4::load(b + i);
        Vec4 vc = va + vb;
        vc.store(c + i);
    }
}

} // namespace hpc::simd
```

## 运行时分发

### 函数多版本化

```cpp
// Generate multiple versions at compile time
namespace {

__attribute__((target("default")))
void add_arrays_impl(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

__attribute__((target("sse2")))
void add_arrays_impl(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        _mm_storeu_ps(c + i, _mm_add_ps(va, vb));
    }
}

__attribute__((target("avx2")))
void add_arrays_impl(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        _mm256_storeu_ps(c + i, _mm256_add_ps(va, vb));
    }
}

} // anonymous namespace

// Public interface - compiler selects best version
inline void add_arrays(float* a, float* b, float* c, size_t n) {
    add_arrays_impl(a, b, c, n);
}
```

### 手动运行时分发

```cpp
#include <cpuid.h>

enum class SIMDCapability {
    Scalar,
    SSE2,
    AVX2,
    AVX512
};

SIMDLevel detect_simd() {
    unsigned int eax, ebx, ecx, edx;

    __get_cpuid(1, &eax, &ebx, &ecx, &edx);

    if (ecx & bit_AVX512F) return SIMDCapability::AVX512;
    if (ecx & bit_AVX2) return SIMDCapability::AVX2;
    if (edx & bit_SSE2) return SIMDCapability::SSE2;

    return SIMDCapability::Scalar;
}

// Function pointer table
using AddFunc = void(*)(float*, float*, float*, size_t);

AddFunc select_add_func() {
    static const AddFunc func = []() -> AddFunc {
        switch (detect_simd()) {
            case SIMDCapability::AVX512: return add_avx512;
            case SIMDCapability::AVX2: return add_avx2;
            case SIMDCapability::SSE2: return add_sse2;
            default: return add_scalar;
        }
    }();
    return func;
}
```

## 掩码操作（AVX-512）

AVX-512 引入了掩码寄存器，支持谓词化执行：

```cpp
// AVX-512 masked operation
void conditional_scale(float* data, size_t n, float threshold) {
    const __m512 thresh = _mm512_set1_ps(threshold);
    const __m512 scale = _mm512_set1_ps(2.0f);

    for (size_t i = 0; i < n; i += 16) {
        __m512 v = _mm512_loadu_ps(data + i);

        // Compare creates a mask
        __mmask16 mask = _mm512_cmp_ps_mask(v, thresh, _MM_CMPINT_GT);

        // Only scale elements where mask is 1
        __m512 scaled = _mm512_mul_ps(v, scale);
        __m512 result = _mm512_mask_blend_ps(mask, v, scaled);

        _mm512_storeu_ps(data + i, result);
    }
}
```

### 掩码的收益

1. **无分支**：消除分支误预测
2. **完整的向量利用率**：所有 lane 均活跃，仅执行不同操作
3. **更清晰的代码**：条件逻辑的表达更自然

## 性能分析

### 吞吐对比

| 操作 | Scalar | SSE2 | AVX2 | AVX-512 |
|-----------|--------|------|------|---------|
| 加法 (GFLOPS) | 2.0 | 8.0 | 16.0 | 32.0 |
| 乘法 (GFLOPS) | 2.0 | 8.0 | 16.0 | 32.0 |
| FMA (GFLOPS) | N/A | N/A | 32.0 | 64.0 |

### 延迟与吞吐

```cpp
// High latency operation - hide with independent work
__m256 x = _mm256_div_ps(a, b);  // ~10-15 cycles

// Do independent work while division completes
__m256 y = _mm256_mul_ps(c, d);  // ~5 cycles
__m256 z = _mm256_add_ps(e, f);  // ~3 cycles

// Now use x - latency hidden
```

## 实践指南

### 何时使用显式 SIMD

✅ **使用显式 SIMD 的场景：**
- 自动向量化失败（检查编译器报告）
- 性能至关重要且向量化是关键
- 需要掩码操作（AVX-512）
- 算法具有特定的 SIMD 友好结构

❌ **优先使用自动向量化的场景：**
- 代码简洁性重要
- 编译器能有效向量化
- 跨架构可移植性重要
- 维护成本是一个考量因素

### 调试向量化

```bash
# GCC: Show vectorization reports
g++ -O3 -fopt-info-vec-missed
g++ -O3 -fopt-info-vec-optimized

# Clang: Show optimization remarks
clang++ -O3 -Rpass=loop-vectorize
clang++ -O3 -Rpass-missed=loop-vectorize

# Intel: Detailed vectorization report
icpc -O3 -qopt-report=5 -qopt-report-phase=vec
```

## 参考文献

<Citation
  :references="[
    {
      id: 'fog2023',
      author: 'Fog, A.',
      title: 'Optimizing software in C++ - Chapter 12: Vectorization',
      year: 2023,
      source: 'Copenhagen University',
      url: 'https://www.agner.org/optimize/'
    },
    {
      id: 'intel2023',
      author: 'Intel',
      title: 'Intel Intrinsics Guide',
      year: 2023,
      source: 'Intel Corporation',
      url: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html'
    },
    {
      id: 'autovec2023',
      author: 'GCC Team',
      title: 'GCC Auto-vectorization documentation',
      year: 2023,
      source: 'GCC Manual',
      url: 'https://gcc.gnu.org/projects/tree-ssa/vectorization.html'
    }
  ]"
/>
