# 内存布局与缓存

> 本文所有示例均可在本仓库中直接编译运行。运行基准测试查看你的硬件上的实际数据。

## 为什么内存布局决定性能

现代 CPU 的运算速度远超内存带宽。处理器通过多级缓存弥合这一鸿沟：

| 层级 | 典型容量 | 访问延迟 | 带宽 |
|------|----------|----------|------|
| L1 Data Cache | 32–64 KB | ~1 ns (4 cycles) | ~2 TB/s |
| L2 Cache | 256 KB – 1 MB | ~4 ns (12 cycles) | ~1 TB/s |
| L3 Cache (shared) | 8–64 MB | ~10–20 ns (40–80 cycles) | ~400 GB/s |
| Main Memory (DDR5) | 16–256 GB | ~80–120 ns (200–300 cycles) | ~50–100 GB/s |

**关键洞察：一次 L1 命中约 4 个时钟周期，一次 DRAM 访问约 200–300 个周期——代价相差近 100 倍。**

这意味着数据局部性是性能的第一决定因素。相同算法、相同数据量，缓存命中率不同，实测性能可以差 5–10 倍。而内存布局直接决定了访问模式——相同数据不同排列，缓存行为截然不同。

本仓库 `include/hpc/core.hpp` 提供平台常量：

```cpp
namespace hpc::core {
constexpr std::size_t CACHE_LINE_SIZE = 64;  // 编译期常量，用于 alignas

inline std::size_t cache_line_size() {       // 运行时检测
#if defined(__cpp_lib_hardware_interference_size)
    return std::hardware_destructive_interference_size;
#else
    return 64;
#endif
}
}
```

---

## AOS vs SOA

### 问题

粒子系统中每个粒子有位置 `(x, y, z)` 和速度 `(vx, vy, vz)`。AOS 是最自然的写法：

```cpp
// include/hpc/particle_types.hpp
struct ParticleAOS {
    float x, y, z;     // Position
    float vx, vy, vz;  // Velocity
};
// 内存布局: [x,y,z,vx,vy,vz][x,y,z,vx,vy,vz][x,y,z,vx,vy,vz]...
```

更新位置时，每个 cache line（64B）装约 2.67 个粒子（每个 24B），但只需 12B 位置数据——**缓存利用率不到 50%**，且跨字段访问阻止编译器向量化。

### SOA 方案

```cpp
// include/hpc/particle_types.hpp
struct ParticleSOA {
    std::vector<float> x, y, z;     // Positions
    std::vector<float> vx, vy, vz;  // Velocities
};

inline void update_particles_soa(ParticleSOA& particles, float dt) {
    const size_t n = particles.size();
    for (size_t i = 0; i < n; ++i) particles.x[i] += particles.vx[i] * dt;
    for (size_t i = 0; i < n; ++i) particles.y[i] += particles.vy[i] * dt;
    for (size_t i = 0; i < n; ++i) particles.z[i] += particles.vz[i] * dt;
}
```

每个数组连续存储，一个 cache line 装 16 个 float，利用率 100%，且编译器可自动向量化。

| 维度 | AOS | SOA |
|------|-----|-----|
| 缓存利用率（仅访问位置） | ~50% | ~100% |
| 自动向量化 | 困难 | 容易 |
| 随机访问单个粒子 | 好 | 差（多次访问） |
| 适用场景 | 游戏对象、OOP | 物理模拟、列式处理 |

### 运行验证

```bash
./build/release/examples/02-memory-cache/src/aos_vs_soa
./build/release/examples/02-memory-cache/bench/aos_soa_bench
```

SOA 的优势在数据量超过 L2 缓存后尤为显著。

---

## Cache Line 对齐

### 为什么对齐重要

当 SIMD 指令操作未对齐地址时：跨 cache line 的 32 字节 AVX load 需要两次缓存查找；对齐指令在某些微架构上比非对齐少一个解码 μop。

### 仓库示例

`examples/02-memory-cache/src/alignment.cpp` 对比对齐与非对齐 AVX 加载：

```cpp
constexpr size_t SIMD_ALIGNMENT = 32;  // AVX alignment

// 对齐版本
void add_avx_aligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_load_ps(a + i);   // 要求 32 字节对齐
        __m256 vb = _mm256_load_ps(b + i);
        _mm256_store_ps(c + i, _mm256_add_ps(va, vb));
    }
    for (; i < n; ++i) c[i] = a[i] + b[i];
}

// 非对齐版本
void add_avx_unaligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);  // 无对齐要求
        __m256 vb = _mm256_loadu_ps(b + i);
        _mm256_storeu_ps(c + i, _mm256_add_ps(va, vb));
    }
    for (; i < n; ++i) c[i] = a[i] + b[i];
}
```

### 对齐工具

`include/hpc/memory_utils.hpp` 提供跨平台对齐分配和 cache line padding：

```cpp
inline void* aligned_alloc(std::size_t size, std::size_t alignment) {
#if defined(_MSC_VER)
    return _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0) return nullptr;
    return ptr;
#endif
}

template <typename T, std::size_t Alignment = hpc::core::CACHE_LINE_SIZE>
class AlignedAllocator { /* ... */ };

template <typename T>
struct alignas(hpc::core::CACHE_LINE_SIZE) CacheLinePadded {
    T value;
};
```

### 运行验证

```bash
./build/release/examples/02-memory-cache/src/alignment
./build/release/examples/02-memory-cache/bench/alignment_bench
```

在现代 CPU（Skylake+）上对齐与非对齐差距已很小，但对齐仍是最佳实践——确保所有微架构上无性能回退。

---

## False Sharing

### 问题本质

不同核心上的线程修改**同一条 cache line 上的不同变量**时，MESI 协议不断使该 cache line 在核心间失效——即使变量逻辑上完全独立。

### 仓库示例

`examples/02-memory-cache/src/false_sharing.cpp`：

```cpp
// 问题：4 个 atomic 挤在同一条 cache line
struct PackedCounters {
    std::atomic<int64_t> counter1{0};
    std::atomic<int64_t> counter2{0};
    std::atomic<int64_t> counter3{0};
    std::atomic<int64_t> counter4{0};
};
// sizeof == 32 bytes → 全部共享一条 cache line

// 修复：alignas(64) 隔离每个计数器
struct alignas(hpc::core::CACHE_LINE_SIZE) PaddedCounter {
    std::atomic<int64_t> value{0};
};

struct PaddedCounters {
    PaddedCounter counter1;
    PaddedCounter counter2;
    PaddedCounter counter3;
    PaddedCounter counter4;
};
// sizeof == 256 bytes → 每个计数器独立 cache line
```

### 通用工具

对任意类型使用 `CacheLinePadded<T>`：

```cpp
// 每个线程的计数器自动隔离到独立 cache line
std::vector<CacheLinePadded<std::atomic<int64_t>>> per_thread_counters(num_threads);
```

### 为什么这很重要

False sharing 可以将多线程加速比从接近线性降到**负加速**。典型场景：per-thread 累加器、无锁队列 head/tail 指针、内存池 per-thread free list。

### 运行验证

```bash
./build/release/examples/02-memory-cache/src/false_sharing
./build/release/examples/02-memory-cache/bench/false_sharing_bench
```

在 4 线程、每线程 10M 次原子递增下，padded 版本通常比 packed 版本快数倍。

---

## 软件预取

### 何时需要

硬件预取器能识别顺序和 stride 模式，但对随机访问、指针追踪无能为力。此时用 `__builtin_prefetch` 显式提示 CPU：

```cpp
__builtin_prefetch(addr, rw, locality);
// rw: 0=读, 1=写; locality: 0=NTA, 3=T0(留在所有层级)
```

### 仓库示例

`examples/02-memory-cache/src/prefetch.cpp` 展示三种场景：

**随机访问（收益明显）：**

```cpp
int64_t sum_random_with_prefetch(const int64_t* data, const size_t* indices, size_t n) {
    constexpr size_t PREFETCH_DISTANCE = 8;
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        if (i + PREFETCH_DISTANCE < n) {
            __builtin_prefetch(&data[indices[i + PREFETCH_DISTANCE]], 0, 3);
        }
        sum += data[indices[i]];
    }
    return sum;
}
```

**指针追踪（链表）：**

```cpp
int64_t sum_list_with_prefetch(const Node* head) {
    int64_t sum = 0;
    for (const Node* node = head; node != nullptr; node = node->next) {
        if (node->next != nullptr) {
            __builtin_prefetch(node->next, 0, 3);
        }
        sum += node->value;
    }
    return sum;
}
```

### 预取距离调优

- **太小**：数据未到就要用，预取无效
- **太大**：数据到了就被驱逐，浪费带宽
- **经验值**：8–32 个元素，取决于循环体计算量和缓存延迟

| 场景 | 硬件预取器 | 软件预取 |
|------|-----------|----------|
| 顺序遍历 | 非常有效 | 通常无额外收益 |
| 随机/间接访问 | 无效 | **有效** |
| 指针追踪 | 无效 | **有效** |

### 运行验证

```bash
./build/release/examples/02-memory-cache/src/prefetch
./build/release/examples/02-memory-cache/bench/prefetch_bench
```

---

## 动手验证

本模块所有可运行的 benchmark：

```bash
# 编译
cmake --preset=release && cmake --build --preset=release

# AOS vs SOA
./build/release/examples/02-memory-cache/bench/aos_soa_bench

# 内存对齐
./build/release/examples/02-memory-cache/bench/alignment_bench

# False Sharing
./build/release/examples/02-memory-cache/bench/false_sharing_bench

# 软件预取
./build/release/examples/02-memory-cache/bench/prefetch_bench
```

支持 Google Benchmark 标准参数：

```bash
./build/release/examples/02-memory-cache/bench/aos_soa_bench --benchmark_filter="SOA"
./build/release/examples/02-memory-cache/bench/false_sharing_bench --benchmark_repetitions=5
```

---

## 参考文献

- Ulrich Drepper, "What Every Programmer Should Know About Memory", 2007. 缓存层次结构基本原理和 false sharing 分析方法的经典参考。
- Agner Fog, "Optimizing software in C++" / "The microarchitecture of Intel, AMD and VIA CPUs", 2023. 各微架构精确延迟和吞吐量数据。
- Intel 64 and IA-32 Architectures Optimization Reference Manual. 官方优化指南，含 prefetch 指令使用建议。
- Travis Downs, "Performance Matters", Strange Loop 2019. 正确进行性能测量的实践指导。
