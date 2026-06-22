# SIMD 练习

练习自动向量化、intrinsic 与 SIMD wrapper 的使用。

---

## 练习 1：启用自动向量化

### 目标

编写编译器能够自动向量化的代码。

### 准备

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

### 任务

1. **编译并查看向量化报告**
   ```bash
   g++ -O3 -march=native -fopt-info-vec-optimized auto_vectorize_exercise.cpp
   ```

2. **检查是否已向量化**

3. **若未向量化，则修改代码**以启用向量化

### 问题

<details>
<summary>Q：什么会阻碍自动向量化？</summary>

常见阻碍因素：
- 指针别名（编译器无法证明 a、b、c 不重叠）
- 循环携带依赖
- 未知循环次数
- 复杂控制流
- 循环内的函数调用
</details>

### 提示

<details>
<summary>点击查看提示</summary>

添加 `__restrict` 告诉编译器指针之间不重叠：

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

## 练习 2：将标量代码转换为 SIMD

### 目标

将标量函数转换为使用 AVX2 intrinsic 的版本。

### 准备

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

### 任务

1. **使用 `_mm256_*` intrinsic 实现 AVX2 版本**

2. **处理尾部余数**（不能被 8 整除的元素）

3. **对两个版本进行 benchmark**
   ```bash
   g++ -O3 -march=native -mavx2 scale_exercise.cpp -o scale_exercise
   ./scale_exercise
   ```

### 参考

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

### 解答骨架

<details>
<summary>点击查看解答骨架</summary>

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

## 练习 3：SIMD wrapper 的使用

### 目标

使用提供的 SIMD wrapper 编写更简洁的代码。

### 任务

1. **使用 `hpc::simd::FloatVec` 重写练习 2**

2. **比较可读性**

3. **对两种实现进行 benchmark**

### 参考

```cpp
#include <hpc/simd.hpp>

using namespace hpc::simd;

FloatVec v(&arr[i]);           // Load
FloatVec vscalar(scalar);       // Broadcast
FloatVec result = v * vscalar;  // Multiply
result.store(&arr[i]);          // Store
```

---

## 练习 4：水平操作

### 目标

使用 SIMD 实现点积。

### 准备

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

### 提示

<details>
<summary>点击查看提示</summary>

使用 FMA（fused multiply-add）提高效率：
1. 在 SIMD 向量中累加乘积
2. 最后使用 `horizontal_sum()` 求和
3. 单独处理尾部余数元素
</details>

---

## 练习 5：比较操作

### 目标

使用 SIMD min/max 操作实现 clamp。

### 任务

1. **实现 `clamp_array`**，将值限制在 [min_val, max_val] 范围内

2. **使用 SIMD min/max intrinsic**

3. **与标量版本进行 benchmark 对比**

### 参考

```cpp
__m256 _mm256_min_ps(__m256 a, __m256 b);
__m256 _mm256_max_ps(__m256 a, __m256 b);
```

---

## 挑战：优化一个真实算法

### 目标

使用 SIMD 优化 Mandelbrot 集合计算。

### 准备

Mandelbrot 计算是 embarrassingly parallel 的——每个像素相互独立。

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

1. **向量化**，同时处理 8 个像素

2. **处理控制流**（发散时的提前退出）

3. **测量加速比**

### 提示

<details>
<summary>点击查看提示</summary>

由于 SIMD 不支持按 lane 提前退出，使用 mask 来跟踪哪些像素已经发散：

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

## 解答

完整解答见 [solutions.md](solutions.md)。

---

## 后续步骤

- 继续 [Concurrency 练习](module-05-concurrency.md)
- 阅读 [SIMD Wrapper API](../reference/api/simd-wrapper.md)
- 探索 [优化决策树](../guides/optimization-decision-tree.md)
