# 最佳实践

工业级测试过的高性能 C++ 编程模式和推荐做法。

---

## 目录

1. [代码组织](#代码组织)
2. [内存管理](#内存管理)
3. [缓存优化](#缓存优化)
4. [SIMD 与向量化](#simd-与向量化)
5. [并发编程](#并发编程)
6. [基准测试](#基准测试)

---

## 代码组织

### 命名空间组织

使用项目特定命名空间避免冲突：

```cpp
namespace hpc {
    namespace memory {
        // 内存相关工具
    }
    namespace simd {
        // SIMD 操作
    }
    namespace concurrent {
        // 并发原语
    }
}
```

### 头文件保护

使用 `#pragma once`（现代）或传统包含保护：

```cpp
#pragma once

// 或

#ifndef HPC_MODULE_FILENAME_HPP
#define HPC_MODULE_FILENAME_HPP
// ...
#endif // HPC_MODULE_FILENAME_HPP
```

### CMake 结构

- 每个模块一个 `CMakeLists.txt`
- 使用 `target_` 命令（非全局命令）
- 显式声明依赖

---

## 内存管理

### 优先使用栈分配

```cpp
// 好: 栈分配
void process() {
    std::array<int, 100> buffer;  // 快速、缓存友好
}

// 避免: 不必要的堆分配
void process() {
    auto buffer = std::make_unique<int[]>(100);  // 较慢
}
```

### 使用智能指针

```cpp
// 所有权语义
std::unique_ptr<Resource> owner;      // 独占所有权
std::shared_ptr<Resource> shared;     // 共享所有权
std::weak_ptr<Resource> weak_ref;     // 非所有者引用
```

### SIMD 对齐

```cpp
// 为 SIMD 操作对齐数据
alignas(64) float buffer[256];  // 64字节对齐以支持 AVX-512

// 或使用对齐分配
auto* ptr = static_cast<float*>(
    std::aligned_alloc(64, size * sizeof(float))
);
```

---

## 缓存优化

### 数据结构布局

```cpp
// SOA (数组结构体) - 对 SIMD/缓存更友好
struct ParticleSystem {
    std::vector<float> pos_x, pos_y, pos_z;
    std::vector<float> vel_x, vel_y, vel_z;
    std::vector<float> mass;
};

// AOS (结构体数组) - 对 OOP/访问模式更友好
struct Particle {
    float pos[3], vel[3], mass;
};
std::vector<Particle> particles;
```

### 访问模式

```cpp
// 好: 顺序访问
for (size_t i = 0; i < n; ++i) {
    data[i] = compute(i);
}

// 差: 随机访问
for (size_t i = 0; i < n; ++i) {
    data[random_idx[i]] = compute(i);
}
```

### 伪共享预防

```cpp
// 填充到缓存行大小以防止伪共享
struct alignas(64) Counter {
    std::atomic<size_t> value{0};
    char padding[64 - sizeof(std::atomic<size_t>)];
};

std::array<Counter, num_threads> counters;
```

---

## SIMD 与向量化

### 编译器提示

```cpp
// 启用自动向量化
void process(float* __restrict a, 
             const float* __restrict b, 
             size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] += b[i];  // 编译器可向量化
    }
}
```

### 显式 SIMD

```cpp
#include <immintrin.h>

void add_avx2(float* dst, const float* a, const float* b, size_t n) {
    size_t i = 0;
    // 一次处理 8 个 float
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&dst[i], vc);
    }
    // 处理剩余部分
    for (; i < n; ++i) {
        dst[i] = a[i] + b[i];
    }
}
```

---

## 并发编程

### 线程安全

```cpp
// 线程安全初始化
std::once_flag init_flag;
void initialize() {
    std::call_once(init_flag, []() {
        // 一次性初始化
    });
}

// 原子操作
std::atomic<size_t> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);
```

### 内存序

```cpp
// seq_cst: 最强，默认
atomic.store(value);  // seq_cst

// Release/Acquire: 用于同步
atomic.store(value, std::memory_order_release);
auto val = atomic.load(std::memory_order_acquire);

// Relaxed: 仅用于计数器
counter.fetch_add(1, std::memory_order_relaxed);
```

### 无锁数据结构

```cpp
// 单生产者单消费者队列
template<typename T, size_t Size>
class SPSCQueue {
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    alignas(64) std::array<T, Size> buffer_;
    // ...
};
```

---

## 基准测试

### Google Benchmark 最佳实践

```cpp
#include <benchmark/benchmark.h>

// 防止优化
static void BM_vector_pushback(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v.data());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_vector_pushback)->Range(8, 8<<10);

// 统计显著性
BENCHMARK(BM_example)
    ->Repetitions(10)
    ->ReportAggregatesOnly(true);
```

### 环境控制

```bash
# 禁用 CPU 频率动态调整
sudo cpupower frequency-set --governor performance

# 绑定到特定 CPU
taskset -c 0 ./benchmark

# 禁用 ASLR 提高可复现性
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

---

## 参考

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)（英文）
- [Optimizing Software in C++](https://agner.org/optimize/optimizing_cpp.pdf)（英文）
- [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)（英文）
