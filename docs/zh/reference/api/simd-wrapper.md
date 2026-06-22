# SIMD Wrapper API

为 SIMD intrinsic 提供的 C++ wrapper，为向量化运算提供简洁、可移植的接口。

---

## 概览

**头文件：** `include/hpc/simd.hpp`

**命名空间：** `hpc::simd`

---

## SIMD 级别检测

该库在编译期自动检测可用的 SIMD 指令集：

| 宏 | 指令集 | 宽度 |
|-------|-----------------|-------|
| `HPC_HAS_SSE2` | SSE2 | 128 位（4 个 float） |
| `HPC_HAS_AVX` | AVX | 256 位（8 个 float） |
| `HPC_HAS_AVX2` | AVX2 | 256 位（8 个 float） |
| `HPC_HAS_AVX512` | AVX-512 | 512 位（16 个 float） |

---

## SimdVec 类

### 模板参数

```cpp
template<typename T, size_t Width>
class SimdVec;
```

- `T` - 元素类型（目前对 `float` 做了特化）
- `Width` - 元素数量（4、8 或 16）

---

## 通用接口

所有 SIMD 向量类型共享以下接口：

#### 构造

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

#### 存储

```cpp
// Store to unaligned memory
void store(float* ptr) const;

// Store to aligned memory
void store_aligned(float* ptr) const;
```

#### 元素访问

```cpp
// Get element at index (slow, for debugging)
float operator[](size_t i) const;
```

#### 算术运算符

```cpp
SimdVec operator+(const SimdVec& other) const;
SimdVec operator-(const SimdVec& other) const;
SimdVec operator*(const SimdVec& other) const;
SimdVec operator/(const SimdVec& other) const;

SimdVec& operator+=(const SimdVec& other);
SimdVec& operator-=(const SimdVec& other);
SimdVec& operator*=(const SimdVec& other);
```

#### 数学运算

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

## 类型别名

### FloatVec

```cpp
using FloatVec = SimdVec<float, WIDTH>;  // WIDTH depends on available SIMD
```

默认 SIMD 向量类型，自动选择可用的最宽指令集。

| 可用 SIMD | FloatVec 宽度 |
|----------------|----------------|
| AVX-512 | 16 个 float |
| AVX2 | 8 个 float |
| SSE2 | 4 个 float |
| 无 | 4 个 float（标量回退） |

### FLOAT_VEC_WIDTH

```cpp
constexpr size_t FLOAT_VEC_WIDTH;  // 4, 8, or 16
```

默认 `FloatVec` 类型中 float 的数量。

---

## 高层操作

### add_arrays_wrapped

```cpp
void add_arrays_wrapped(const float* a, const float* b, float* c, size_t n);
```

逐元素相加两个数组：`c[i] = a[i] + b[i]`

**示例：**
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

计算点积：`sum(a[i] * b[i])`

**示例：**
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

以标量缩放数组：`arr[i] *= scalar`

---

### clamp_array_wrapped

```cpp
void clamp_array_wrapped(float* arr, float min_val, float max_val, size_t n);
```

将数组值限制在范围内：`arr[i] = clamp(arr[i], min_val, max_val)`

---

## 用法示例

### 基本向量运算

```cpp
#include <hpc/simd.hpp>

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

### 使用融合乘加

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

### 使用对齐内存以获得最佳性能

```cpp
#include "memory_utils.hpp"
#include <hpc/simd.hpp>

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

## 性能注意事项

### 对齐

为获得最佳性能：
- 当数据为 64 字节对齐时，使用 `load_aligned()` 和 `store_aligned()`
- 对齐加载在某些架构上可避免额外指令

### 余数处理

始终处理无法填满完整 SIMD 向量的剩余元素：

```cpp
size_t i = 0;
for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
    // SIMD loop
}
for (; i < n; ++i) {
    // Scalar remainder
}
```

### 内存带宽

SIMD 在以下情况最为有利：
- 数据已在 cache 中（内存带宽受限的操作收益较小）
- 运算为计算密集型
- 数据访问是顺序的

---

## 快速参考

| 运算 | 方法 | SIMD 等价物 |
|-----------|--------|-----------------|
| 加法 | `a + b` | `_mm_add_ps` |
| 减法 | `a - b` | `_mm_sub_ps` |
| 乘法 | `a * b` | `_mm_mul_ps` |
| 除法 | `a / b` | `_mm_div_ps` |
| FMA | `fmadd(a,b,c)` | `_mm_fmadd_ps` |
| 平方根 | `a.sqrt()` | `_mm_sqrt_ps` |
| 水平求和 | `a.horizontal_sum()` | 手动归约 |
| 最小值 | `a.min(b)` | `_mm_min_ps` |
| 最大值 | `a.max(b)` | `_mm_max_ps` |

---

## 另请参阅

- [SIMD 模块示例](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/04-simd-vectorization)
- [Memory 工具 API](memory-utils.md) - 用于对齐分配
- [优化决策树](../../guides/optimization-decision-tree.md)
