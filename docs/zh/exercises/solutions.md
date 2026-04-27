# 练习解答

交互式练习的解答和提示。

---

## 如何使用本页

1. **先尝试练习** - 在尝试过之前不要阅读解答
2. **在解答前使用提示** - 提示可以指导而不直接给出答案
3. **理解，不要复制** - 确保你理解解答*为什么*有效

---

## 内存练习解答

### 练习 1：AOS 到 SOA 转换

<details>
<summary>解答</summary>

```cpp
// 带有 mass 字段的 AOS 版本
struct ParticleAOS {
    float x, y, z;
    float vx, vy, vz;
    float mass;  // 新字段
};

// 带有 mass 字段的 SOA 版本
struct ParticleSOA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;  // 新字段
};

// SOA 的更新函数（更适合按字段访问）
void update_positions_soa(ParticleSOA& p, float dt, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        p.x[i] += p.vx[i] * dt;
        p.y[i] += p.vy[i] * dt;
        p.z[i] += p.vz[i] * dt;
    }
}
```

即使添加了新字段，SOA 版本仍保持按字段操作的缓存效率。
</details>

### 练习 2：修复伪共享

<details>
<summary>解答</summary>

```cpp
#include <atomic>
#include <thread>
#include <vector>

// 方案 1：使用 alignas
struct alignas(64) AlignedCounter {
    std::atomic<int> value{0};
    // 由于 alignas(64)，填充是自动的
};

void increment_counters(AlignedCounter* counters, int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counters[thread_id].value.fetch_add(1, std::memory_order_relaxed);
    }
}

// 方案 2：使用 memory_utils.hpp 中的 CacheLinePadded
#include "memory_utils.hpp"

void increment_counters_padded(hpc::memory::CacheLinePadded<std::atomic<int>>* counters,
                               int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counters[thread_id].value.fetch_add(1, std::memory_order_relaxed);
    }
}
```

预期加速：2-10 倍，取决于线程数和 CPU 架构。
</details>

---

## SIMD 练习解答

### 练习 1：启用自动向量化

<details>
<summary>解答</summary>

```cpp
// 添加 __restrict 告诉编译器指针不重叠
void add_arrays(float* __restrict a, 
                const float* __restrict b, 
                const float* __restrict c, 
                size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}

// 替代方案：在每个指针上单独使用 restrict
void add_arrays(float* restrict a, 
                const float* restrict b, 
                const float* restrict c, 
                size_t n);
```

使用 `g++ -O3 -march=native -fopt-info-vec-optimized` 编译以验证向量化。
</details>

### 练习 2：标量转 SIMD

<details>
<summary>解答</summary>

```cpp
#include <immintrin.h>

void scale_array_avx2(float* arr, float scalar, size_t n) {
    // 将标量广播到所有 8 个通道
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

对于对齐数组，使用 `_mm256_load_ps` 和 `_mm256_store_ps` 可能获得更好的性能。
</details>

### 练习 4：点积

<details>
<summary>解答</summary>

```cpp
#include <immintrin.h>

float dot_product_avx2(const float* a, const float* b, size_t n) {
    __m256 sum_vec = _mm256_setzero_ps();
    
    size_t i = 0;
    
    // 每次处理 8 个元素
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // FMA：sum_vec = va * vb + sum_vec
        sum_vec = _mm256_fmadd_ps(va, vb, sum_vec);
    }
    
    // 8 个通道的水平求和
    // 首先，提取高 128 位和低 128 位
    __m128 hi = _mm256_extractf128_ps(sum_vec, 1);
    __m128 lo = _mm256_castps256_ps128(sum_vec);
    __m128 sum128 = _mm_add_ps(hi, lo);
    
    // 打乱并相加
    __m128 shuf = _mm_shuffle_ps(sum128, sum128, _MM_SHUFFLE(2, 3, 0, 1));
    sum128 = _mm_add_ps(sum128, shuf);
    shuf = _mm_movehl_ps(shuf, sum128);
    sum128 = _mm_add_ss(sum128, shuf);
    
    float result = _mm_cvtss_f32(sum128);
    
    // 处理剩余元素
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}
```

使用 SIMD 包装器（更清晰）：

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

## 并发练习解答

### 练习 1：调试数据竞争

<details>
<summary>解答</summary>

```cpp
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

// 解答：使用原子变量
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
    
    std::cout << "期望值：" << threads * increments << "\n";
    std::cout << "实际值：" << counter.load() << "\n";
    return 0;
}
```

关键修复：
1. 将 `int` 改为 `std::atomic<int>`
2. 使用 `fetch_add` 代替 `++`
3. 使用 `memory_order_relaxed`，因为我们只需要原子性，不需要顺序
</details>

### 练习 3：线程安全计数器

<details>
<summary>解答</summary>

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
    // alignas 确保与相邻数据不发生伪共享
    alignas(64) std::atomic<int> value_{0};
};

// 对于多个不会伪共享的计数器：
struct CounterArray {
    alignas(64) std::atomic<int> counter0{0};
    alignas(64) std::atomic<int> counter1{0};
    alignas(64) std::atomic<int> counter2{0};
    alignas(64) std::atomic<int> counter3{0};
};
```
</details>

---

## 挑战解答

### Mandelbrot SIMD

<details>
<summary>解答（部分）</summary>

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
                
                // 检查是否仍有界
                __m256 mask = _mm256_cmp_ps(r2, four_vec, _CMP_LE_OS);
                if (_mm256_testz_ps(mask, mask)) {
                    break;  // 所有像素已发散
                }
                
                // 只更新活跃像素
                __m256 temp = _mm256_add_ps(
                    _mm256_sub_ps(x2, y2), cx_vec);
                y_vec = _mm256_add_ps(
                    _mm256_mul_ps(
                        _mm256_mul_ps(_mm256_set1_ps(2.0f), x_vec), 
                        y_vec), 
                    cy_vec);
                x_vec = temp;
                
                // 为活跃像素增加迭代计数
                iter_vec = _mm256_add_ps(iter_vec, 
                    _mm256_and_ps(one_vec, mask));
            }
            
            // 存储结果
            _mm256_storeu_ps(reinterpret_cast<float*>(&result[y * width + x]), 
                            iter_vec);
        }
    }
}
```

注意：这是简化版本。完整解答需要正确处理结果存储，可能还需要额外的优化。
</details>

---

## 成功技巧

1. **始终测量** - 不要假设优化有效
2. **使用消毒器** - TSAN 能捕获你可能遗漏的数据竞争
3. **检查编译器输出** - 验证向量化确实发生
4. **先分析** - 在优化前找到瓶颈
5. **理解硬件** - 缓存、内存、CPU 架构很重要

---

## 需要更多帮助？

- [FAQ](../reference/faq.md)
- [故障排除](../reference/troubleshooting.md)
- [GitHub 讨论](https://github.com/LessUp/cpp-high-performance-guide/discussions)
