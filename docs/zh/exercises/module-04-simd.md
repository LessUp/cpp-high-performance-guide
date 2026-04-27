# SIMD 练习

实践自动向量化、内联函数和 SIMD 包装器的使用。

---

## 练习 1：启用自动向量化

### 目标

编写编译器可以自动向量化的代码。

### 准备

```cpp
// 文件：auto_vectorize_exercise.cpp
#include <vector>
#include <cstddef>

// TODO：使此代码可向量化
void add_arrays(float* a, const float* b, const float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}
```

### 任务

1. **使用向量化报告编译**
   ```bash
   g++ -O3 -march=native -fopt-info-vec-optimized auto_vectorize_exercise.cpp
   ```

2. **检查是否已向量化**

3. **如果没有，修改代码** 以启用向量化

### 问题

<details>
<summary>Q：什么阻止了自动向量化？</summary>

常见阻碍因素：
- 指针别名（编译器无法证明 a、b、c 不重叠）
- 循环依赖
- 未知循环次数
- 复杂控制流
- 循环内的函数调用
</details>

### 提示

<details>
<summary>点击查看提示</summary>

添加 `__restrict` 告诉编译器指针不重叠：

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

## 练习 2：标量转 SIMD

### 目标

将标量函数转换为使用 AVX2 内联函数。

### 准备

```cpp
// 标量版本
void scale_array_scalar(float* arr, float scalar, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

// TODO：实现 AVX2 版本
void scale_array_avx2(float* arr, float scalar, size_t n) {
    // 你的代码
}
```

### 任务

1. **使用 `_mm256_*` 内联函数实现 AVX2 版本**

2. **处理剩余元素**（不能被 8 整除的部分）

3. **对两个版本进行基准测试**
   ```bash
   g++ -O3 -march=native -mavx2 scale_exercise.cpp -o scale_exercise
   ./scale_exercise
   ```

### 参考

```cpp
#include <immintrin.h>

// 加载 8 个浮点数（未对齐）
__m256 _mm256_loadu_ps(const float* addr);

// 存储 8 个浮点数（未对齐）
void _mm256_storeu_ps(float* addr, __m256 a);

// 将标量广播到所有通道
__m256 _mm256_set1_ps(float a);

// 乘法
__m256 _mm256_mul_ps(__m256 a, __m256 b);
```

### 解答框架

<details>
<summary>点击查看解答框架</summary>

```cpp
void scale_array_avx2(float* arr, float scalar, size_t n) {
    __m256 vscalar = _mm256_set1_ps(scalar);
    
    size_t i = 0;
    // 每次处理 8 个浮点数
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        v = _mm256_mul_ps(v, vscalar);
        _mm256_storeu_ps(&arr[i], v);
    }
    
    // 处理剩余元素
    for (; i < n; ++i) {
        arr[i] *= scalar;
    }
}
```
</details>

---

## 练习 3：SIMD 包装器使用

### 目标

使用提供的 SIMD 包装器编写更清晰的代码。

### 任务

1. **使用 `hpc::simd::FloatVec` 重写练习 2**

2. **比较可读性**

3. **对两种实现进行基准测试**

### 参考

```cpp
#include "simd_wrapper.hpp"

using namespace hpc::simd;

FloatVec v(&arr[i]);           // 加载
FloatVec vscalar(scalar);       // 广播
FloatVec result = v * vscalar;  // 乘法
result.store(&arr[i]);          // 存储
```

---

## 练习 4：水平操作

### 目标

使用 SIMD 实现点积。

### 准备

```cpp
// 标量版本
float dot_product_scalar(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// TODO：实现 SIMD 版本
float dot_product_simd(const float* a, const float* b, size_t n) {
    // 你的代码
}
```

### 提示

<details>
<summary>点击查看提示</summary>

使用 FMA（融合乘加）提高效率：
1. 在 SIMD 向量中累积乘积
2. 最后使用 `horizontal_sum()`
3. 单独处理剩余元素
</details>

---

## 练习 5：比较操作

### 目标

使用 SIMD min/max 操作实现截断。

### 任务

1. **实现 `clamp_array`**，将值限制在 [min_val, max_val] 范围内

2. **使用 SIMD min/max 内联函数**

3. **与标量版本进行基准测试对比**

### 参考

```cpp
__m256 _mm256_min_ps(__m256 a, __m256 b);
__m256 _mm256_max_ps(__m256 a, __m256 b);
```

---

## 挑战：优化真实算法

### 目标

使用 SIMD 优化 Mandelbrot 集合计算。

### 准备

Mandelbrot 计算具有令人尴尬的并行性——每个像素是独立的。

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

### 任务

1. **向量化** 以同时处理 8 个像素

2. **处理控制流**（发散时提前退出）

3. **测量加速**

### 提示

<details>
<summary>点击查看提示</summary>

由于 SIMD 不支持每通道提前退出，使用掩码跟踪哪些像素已发散：

```cpp
__m256 cx_vec, cy_vec;  // 8 个 c 值
__m256 x_vec = _mm256_setzero_ps();
__m256 y_vec = _mm256_setzero_ps();
__m256 iter_vec = _mm256_setzero_ps();
__m256 active_mask = _mm256_castsi256_ps(_mm256_set1_epi32(-1));

for (int iter = 0; iter < max_iter; ++iter) {
    // 只处理活跃像素
    // ...
}
```
</details>

---

## 解答

完整解答请参阅 [solutions.md](solutions.md)。

---

## 下一步

- 继续学习 [并发练习](module-05-concurrency.md)
- 阅读 [SIMD 包装器 API](../reference/api/simd-wrapper.md)
- 探索 [优化决策树](../guides/optimization-decision-tree.md)
