# SIMD 向量化

> 本文所有示例均可在本仓库中直接编译运行。运行基准测试查看你的硬件上的实际数据。

## SIMD 的价值

SIMD（Single Instruction, Multiple Data）一条指令同时处理多个数据元素。对数据并行计算——数组运算、信号处理、物理模拟——这是最直接的加速手段。

标量代码每次处理一个元素（4 条加法指令处理 4 个 float）；SIMD 一条指令处理 4/8/16 个元素。

### x86 SIMD 指令集

| ISA 扩展 | 寄存器宽度 | float 并行度 | 引入年份 | 关键特性 |
|----------|-----------|-------------|----------|----------|
| SSE2 | 128-bit (xmm) | 4 | 2001 | 基础浮点 SIMD |
| AVX | 256-bit (ymm) | 8 | 2011 | 更宽寄存器，3-operand 编码 |
| AVX2 | 256-bit (ymm) | 8 | 2013 | 整数 256-bit，gather，FMA |
| AVX-512 | 512-bit (zmm) | 16 | 2016 | 掩码操作，内建归约 |

**为什么这很重要：** 从 SSE2 到 AVX2 理论峰值翻倍，到 AVX-512 再翻倍。实际加速取决于内存带宽和代码能否充分利用宽度。

---

## 自动向量化

在 `-O3 -march=native` 下，编译器能自动将简单循环转为 SIMD 指令。但对代码模式极其敏感。

### 好的模式

`examples/04-simd-vectorization/src/auto_vectorize.cpp` 展示了可向量化的模式：

```cpp
// 连续访问 + 无依赖 + __restrict → 完美向量化
void add_arrays_vectorizable(const float* __restrict a, const float* __restrict b,
                             float* __restrict c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// FMA 模式 → 编译器生成 vfmadd 指令
void fma_vectorizable(const float* __restrict a, const float* __restrict b,
                      const float* __restrict c, float* __restrict d, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        d[i] = a[i] * b[i] + c[i];
    }
}

// Reduction + OpenMP simd 提示
float sum_array_vectorizable(const float* arr, size_t n) {
    float sum = 0.0f;
#pragma omp simd reduction(+ : sum)
    for (size_t i = 0; i < n; ++i) sum += arr[i];
    return sum;
}

// 无分支条件 → vminps/vmaxps
void clamp_array_vectorizable(float* __restrict arr, float min_val, float max_val, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] = arr[i] < min_val ? min_val : (arr[i] > max_val ? max_val : arr[i]);
    }
}
```

### 坏的模式

```cpp
// 循环间依赖 → 不可向量化
void prefix_sum_not_vectorizable(float* arr, size_t n) {
    for (size_t i = 1; i < n; ++i) arr[i] += arr[i - 1];
}

// 间接索引 → 不可向量化
void indirect_access_not_vectorizable(float* __restrict arr, const int* __restrict indices, size_t n) {
    for (size_t i = 0; i < n; ++i) arr[indices[i]] += 1.0f;
}

// 不可内联的函数调用 → 阻止向量化
void loop_with_call_not_vectorizable(float* arr, size_t n) {
    for (size_t i = 0; i < n; ++i) arr[i] = external_function(arr[i]);
}

// 无 __restrict → 编译器假设别名，可能不向量化
void aliased_pointers_not_vectorizable(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) c[i] = a[i] + b[i];
}
```

### 修复：分支 → 无分支

```cpp
// 修复前：复杂分支
if (arr[i] > 0) {
    if (arr[i] > 10) arr[i] = std::sqrt(arr[i]);
    else arr[i] = arr[i] * arr[i];
} else arr[i] = -arr[i];

// 修复后：条件表达式，可被向量化
float abs_x = std::fabs(x);
float positive_result = (abs_x > 10.0f) ? std::sqrt(abs_x) : x * x;
arr[i] = (x > 0.0f) ? positive_result : abs_x;
```

### 检查向量化报告

```bash
cmake --preset=release -DHPC_VECTORIZE_REPORT=ON
cmake --build --preset=release 2>&1 | grep -i "vectorized"

# 或手动: GCC -fopt-info-vec / Clang -Rpass=loop-vectorize
```

---

## Intrinsics 入门

当自动向量化不够时（需要精确控制指令、对齐、水平归约），使用 intrinsics。

### SSE（128-bit，4 floats）

`examples/04-simd-vectorization/src/intrinsics_intro.cpp`：

```cpp
void add_arrays_sse(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);  // 加载 4 floats
        __m128 vb = _mm_loadu_ps(&b[i]);
        _mm_storeu_ps(&c[i], _mm_add_ps(va, vb));
    }
    for (; i < n; ++i) c[i] = a[i] + b[i];  // 尾部
}
```

**点积（含水平归约）：**

```cpp
float dot_product_sse(const float* a, const float* b, size_t n) {
    __m128 sum = _mm_setzero_ps();
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        sum = _mm_add_ps(sum, _mm_mul_ps(_mm_loadu_ps(&a[i]), _mm_loadu_ps(&b[i])));
    }
    // 水平归约：4 lanes → 1 scalar
    __m128 shuf = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(2, 3, 0, 1));
    sum = _mm_add_ps(sum, shuf);
    shuf = _mm_movehl_ps(shuf, sum);
    sum = _mm_add_ss(sum, shuf);
    float result = _mm_cvtss_f32(sum);
    for (; i < n; ++i) result += a[i] * b[i];
    return result;
}
```

### AVX2（256-bit，8 floats）+ FMA

```cpp
float dot_product_avx2(const float* a, const float* b, size_t n) {
    __m256 sum = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        sum = _mm256_fmadd_ps(va, vb, sum);  // FMA: sum += va * vb
    }
    // 归约：256-bit → 128-bit → scalar
    __m128 hi = _mm256_extractf128_ps(sum, 1);
    __m128 lo = _mm256_castps256_ps128(sum);
    __m128 sum128 = _mm_add_ps(hi, lo);
    // ... shuffle + add_ss
}
```

### AVX-512（512-bit，16 floats）

```cpp
float dot_product_avx512(const float* a, const float* b, size_t n) {
    __m512 sum = _mm512_setzero_ps();
    size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        sum = _mm512_fmadd_ps(_mm512_loadu_ps(&a[i]), _mm512_loadu_ps(&b[i]), sum);
    }
    float result = _mm512_reduce_add_ps(sum);  // 内建归约
    for (; i < n; ++i) result += a[i] * b[i];
    return result;
}
```

### 常用 Intrinsics 速查

| 操作 | SSE (128-bit) | AVX2 (256-bit) | AVX-512 (512-bit) |
|------|--------------|----------------|-------------------|
| 加载 | `_mm_loadu_ps` | `_mm256_loadu_ps` | `_mm512_loadu_ps` |
| 存储 | `_mm_storeu_ps` | `_mm256_storeu_ps` | `_mm512_storeu_ps` |
| 加/乘 | `_mm_add_ps` / `_mm_mul_ps` | `_mm256_add_ps` / `_mm256_mul_ps` | `_mm512_add_ps` / `_mm512_mul_ps` |
| FMA | — | `_mm256_fmadd_ps` | `_mm512_fmadd_ps` |
| 广播 | `_mm_set1_ps` | `_mm256_set1_ps` | `_mm512_set1_ps` |
| 归约 | shuffle + add | extract + shuffle | `_mm512_reduce_add_ps` |

---

## hpc::simd 封装

直接写 intrinsics 冗长且平台相关。`include/hpc/simd.hpp` 提供零开销薄封装。

### SimdVec 模板

```cpp
template <typename T, std::size_t Width> class SimdVec;
```

对每个 ISA 级别有特化：`SimdVec<float, 4>`（SSE2）、`SimdVec<float, 8>`（AVX2）、`SimdVec<float, 16>`（AVX-512），以及通用回退 `SimdVecScalar<T, N>`。

AVX2 特化的接口：

```cpp
template <>
class SimdVec<float, 8> {
public:
    static constexpr std::size_t width = 8;
    __m256 data;

    explicit SimdVec(float value);           // 广播
    explicit SimdVec(const float* ptr);      // 非对齐加载
    static SimdVec load_aligned(const float* ptr);

    void store(float* ptr) const;
    void store_aligned(float* ptr) const;

    SimdVec operator+(const SimdVec&) const;
    SimdVec operator*(const SimdVec&) const;
    SimdVec& operator+=(const SimdVec&);

    float horizontal_sum() const;
    static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c);
    SimdVec sqrt() const;
    SimdVec min(const SimdVec&) const;
    SimdVec max(const SimdVec&) const;
};
```

### FloatVec 别名

编译时选择最优宽度：

```cpp
#ifdef HPC_HAS_AVX512
using FloatVec = SimdVec<float, 16>;
#elif defined(HPC_HAS_AVX2)
using FloatVec = SimdVec<float, 8>;
#elif defined(HPC_HAS_SSE2)
using FloatVec = SimdVec<float, 4>;
#else
using FloatVec = SimdVecScalar<float, 4>;
#endif
```

### 使用封装后的算法

```cpp
inline void add_arrays_wrapped(const float* a, const float* b, float* c, std::size_t n) {
    std::size_t i = 0;
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(a + i), vb(b + i);
        (va + vb).store(c + i);
    }
    for (; i < n; ++i) c[i] = a[i] + b[i];
}

inline float dot_product_wrapped(const float* a, const float* b, std::size_t n) {
    FloatVec sum(0.0f);
    std::size_t i = 0;
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        sum = FloatVec::fmadd(FloatVec(a + i), FloatVec(b + i), sum);
    }
    float result = sum.horizontal_sum();
    for (; i < n; ++i) result += a[i] * b[i];
    return result;
}
```

### 运行时检测

```cpp
inline SIMDLevel detect_simd_level() {
    __builtin_cpu_init();
    if (__builtin_cpu_supports("avx512f")) return SIMDLevel::AVX512;
    if (__builtin_cpu_supports("avx2"))    return SIMDLevel::AVX2;
    if (__builtin_cpu_supports("avx"))     return SIMDLevel::AVX;
    if (__builtin_cpu_supports("sse2"))    return SIMDLevel::SSE2;
    return SIMDLevel::Scalar;
}
```

---

## 运行时分发

### 问题

`-march=native` 生成的代码只能在当前 CPU 运行。分发二进制需要在运行时检测 CPU 能力。

### 仓库实现

`include/hpc/simd.hpp` 使用 GCC/Clang `target` 属性在同一编译单元生成多版本代码：

```cpp
__attribute__((target("sse2")))
inline void add_arrays_sse2(const float* a, const float* b, float* c, std::size_t n) { /* ... */ }

__attribute__((target("avx2,avx")))
inline void add_arrays_avx2(const float* a, const float* b, float* c, std::size_t n) { /* ... */ }

__attribute__((target("avx512f,avx2,avx")))
inline void add_arrays_avx512(const float* a, const float* b, float* c, std::size_t n) { /* ... */ }
```

分发逻辑（函数指针只解析一次）：

```cpp
inline void add_arrays(const float* a, const float* b, float* c, std::size_t n) {
    using Fn = detail::AddArraysFn;
    static const Fn dispatch =
        detail::resolve_best<Fn>(&detail::add_arrays_scalar, &detail::add_arrays_sse2,
                                 &detail::add_arrays_avx2, &detail::add_arrays_avx512);
    dispatch(a, b, c, n);
}
```

**设计要点：**
- `static const` 函数指针：首次调用解析，后续仅一次间接跳转
- 各版本尾部调用下一级宽度实现（AVX-512 尾 → AVX2 → SSE2 → scalar）
- 非 x86 平台自动回退标量

### 使用示例

`examples/04-simd-vectorization/src/dispatch_example_main.cpp`：

```cpp
#include <hpc/simd.hpp>

int main() {
    hpc::simd::aligned_vector<float> a(8), b(8), c(8);
    // ... 初始化 ...
    hpc::simd::dispatch_add_arrays(a.data(), b.data(), c.data(), 8);
}
```

### 编译时 vs 运行时分发

| 维度 | 编译时 (`#ifdef`) | 运行时 (`target` + dispatch) |
|------|-------------------|------------------------------|
| 二进制可移植性 | 差 | 好 |
| 性能开销 | 零 | 一次间接跳转 |
| 代码复杂度 | 多编译单元 | 单文件 |
| 适用场景 | 内部部署 | 分发二进制、库 |

---

## 动手验证

```bash
# 编译
cmake --preset=release && cmake --build --preset=release

# 自动向量化示例
./build/release/examples/04-simd-vectorization/auto_vectorize

# Intrinsics 示例（scalar/SSE/AVX2/AVX-512 对比）
./build/release/examples/04-simd-vectorization/intrinsics_intro

# 运行时分发示例
./build/release/examples/04-simd-vectorization/dispatch_example

# Google Benchmark 基准测试
./build/release/examples/04-simd-vectorization/simd_bench

# 过滤特定测试
./build/release/examples/04-simd-vectorization/simd_bench --benchmark_filter="dot"
```

查看向量化报告：

```bash
cmake --preset=release -DHPC_VECTORIZE_REPORT=ON
cmake --build --preset=release 2>&1 | grep -i "vectorized"
```

运行基准测试查看你的硬件上的实际数据。加速比取决于 CPU 的 ISA 级别、数组大小（是否超出缓存）、以及内存带宽是否成为瓶颈。

---

## 参考文献

- Agner Fog, "Optimizing software in C++" / "Instruction tables", 2023. 各微架构指令延迟和吞吐量的权威参考。
- Intel Intrinsics Guide, https://www.intel.com/content/www/us/en/docs/intrinsics-guide/. 交互式查询所有 x86 SIMD 指令。
- Travis Downs, "Performance Matters", Strange Loop 2019. SIMD 实际性能受限于前端和内存子系统的深入分析。
- Wojciech Muła, Daniel Lemire, "Faster Base64 Encoding and Decoding Using AVX2 Instructions", 2018. AVX2 工程实践。
- GCC Auto-Vectorization documentation, https://gcc.gnu.org/projects/tree-ssa/vectorization.html. 自动向量化器的限制说明。
