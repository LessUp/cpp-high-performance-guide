# SIMD 包装器 API

用于 SIMD 内联函数的 C++ 包装器，提供清晰、可移植的向量化操作接口。

---

## 概述

**头文件：** `examples/04-simd-vectorization/include/simd_wrapper.hpp`

**命名空间：** `hpc::simd`

---

## SIMD 级别检测

库在编译时自动检测可用的 SIMD 指令集：

| 宏 | 指令集 | 宽度 |
|------|--------|------|
| `HPC_HAS_SSE2` | SSE2 | 128 位（4 个浮点数） |
| `HPC_HAS_AVX` | AVX | 256 位（8 个浮点数） |
| `HPC_HAS_AVX2` | AVX2 | 256 位（8 个浮点数） |
| `HPC_HAS_AVX512` | AVX-512 | 512 位（16 个浮点数） |

---

## SimdVec 类

### 模板参数

```cpp
template<typename T, size_t Width>
class SimdVec;
```

- `T` - 元素类型（当前已特化 `float`）
- `Width` - 元素数量（4、8 或 16）

---

### 通用接口

所有 SIMD 向量类型共享此接口：

#### 构造

```cpp
// 默认构造函数 - 零初始化
SimdVec();

// 将单个值广播到所有通道
explicit SimdVec(float val);

// 从未对齐内存加载
SimdVec(const float* ptr);

// 从对齐内存加载（静态方法）
static SimdVec load_aligned(const float* ptr);
```

#### 存储

```cpp
// 存储到未对齐内存
void store(float* ptr) const;

// 存储到对齐内存
void store_aligned(float* ptr) const;
```

#### 元素访问

```cpp
// 获取指定索引的元素（慢，用于调试）
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
// 将所有通道求和为单个值
float horizontal_sum() const;

// 融合乘加：a * b + c
static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c);

// 逐元素平方根
SimdVec sqrt() const;

// 逐元素最小值
SimdVec min(const SimdVec& other) const;

// 逐元素最大值
SimdVec max(const SimdVec& other) const;
```

---

## 类型别名

### FloatVec

```cpp
using FloatVec = SimdVec<float, WIDTH>;  // WIDTH 取决于可用的 SIMD
```

默认 SIMD 向量类型，自动选择最宽的可用指令集。

| 可用 SIMD | FloatVec 宽度 |
|-----------|---------------|
| AVX-512 | 16 个浮点数 |
| AVX2 | 8 个浮点数 |
| SSE2 | 4 个浮点数 |
| 无 | 4 个浮点数（标量回退） |

### FLOAT_VEC_WIDTH

```cpp
constexpr size_t FLOAT_VEC_WIDTH;  // 4、8 或 16
```

默认 `FloatVec` 类型中的浮点数数量。

---

## 高级操作

### add_arrays_wrapped

```cpp
void add_arrays_wrapped(const float* a, const float* b, float* c, size_t n);
```

逐元素相加两个数组：`c[i] = a[i] + b[i]`

**示例：**
```cpp
float a[1024], b[1024], c[1024];
// ... 初始化 a 和 b ...

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
// ... 初始化 ...

float result = hpc::simd::dot_product_wrapped(a, b, 1024);
```

---

### scale_array_wrapped

```cpp
void scale_array_wrapped(float* arr, float scalar, size_t n);
```

数组标量缩放：`arr[i] *= scalar`

---

### clamp_array_wrapped

```cpp
void clamp_array_wrapped(float* arr, float min_val, float max_val, size_t n);
```

将数组值截断到范围：`arr[i] = clamp(arr[i], min_val, max_val)`

---

## 使用示例

### 基本向量操作

```cpp
#include "simd_wrapper.hpp"

using namespace hpc::simd;

void process_arrays(float* a, float* b, float* result, size_t n) {
    size_t i = 0;
    
    // 以 SIMD 宽度为块处理
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(&a[i]);
        FloatVec vb(&b[i]);
        
        // result = a * 2 + b
        FloatVec scaled = va * FloatVec(2.0f);
        FloatVec vr = scaled + vb;
        
        vr.store(&result[i]);
    }
    
    // 处理剩余元素
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
    
    // 处理剩余元素
    for (; i < n; ++i) {
        result += values[i] * weights[i];
    }
    
    return result;
}
```

### 对齐内存以获得最佳性能

```cpp
#include "memory_utils.hpp"
#include "simd_wrapper.hpp"

void aligned_operations() {
    // 分配对齐内存
    auto a = hpc::memory::make_aligned<float>(1024, 64);
    auto b = hpc::memory::make_aligned<float>(1024, 64);
    auto c = hpc::memory::make_aligned<float>(1024, 64);
    
    // ... 初始化 ...
    
    size_t i = 0;
    for (; i + FLOAT_VEC_WIDTH <= 1024; i += FLOAT_VEC_WIDTH) {
        // 使用对齐加载以获得更好的性能
        FloatVec va = FloatVec::load_aligned(&a[i]);
        FloatVec vb = FloatVec::load_aligned(&b[i]);
        FloatVec vc = va + vb;
        vc.store_aligned(&c[i]);
    }
}
```

---

## 性能考虑

### 对齐

为获得最佳性能：
- 当数据是 64 字节对齐时，使用 `load_aligned()` 和 `store_aligned()`
- 对齐加载在某些架构上可以避免额外指令

### 剩余元素处理

始终处理不能放入完整 SIMD 向量的元素：

```cpp
size_t i = 0;
for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
    // SIMD 循环
}
for (; i < n; ++i) {
    // 标量处理剩余元素
}
```

### 内存带宽

SIMD 在以下情况下最有益：
- 数据在缓存中（内存受限的操作收益较小）
- 计算密集型操作
- 顺序数据访问

---

## 快速参考

| 操作 | 方法 | SIMD 等价 |
|------|------|-----------|
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

## 另见

- [SIMD 模块示例](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/04-simd-vectorization)
- [内存工具 API](memory-utils.md) - 用于对齐分配
- [优化决策树](../../guides/optimization-decision-tree.md)
