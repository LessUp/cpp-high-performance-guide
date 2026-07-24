# 优化决策手册

> 优化不是灵感，是诊断。先找到瓶颈，再选择路径。

## 黄金法则

> "We should forget about small efficiencies, say about 97% of the time: premature optimization is the root of all evil. Yet we should not pass up our opportunities in that critical 3%."
> — Donald Knuth

翻译成操作规则：

1. **先测量，后优化**。没有 perf 数据的优化是猜测。
2. **优化最宽的那个函数**。火焰图顶部最宽的平台才是目标。
3. **每次只改一个变量**。否则无法归因。
4. **改完必须验证**。Sanitizer + benchmark 对比，缺一不可。

完整的测量→分析→优化→验证流程见 [性能分析实战](./profiling.md)。

---

## 诊断清单

在动手优化之前，先确定瓶颈类型。用 `perf stat` 跑一次目标程序，对照下表：

### CPU 瓶颈

| 症状 | perf 命令 | 判断阈值 |
|------|-----------|----------|
| IPC 低 | `perf stat -e cycles,instructions <binary>` | IPC < 0.5 |
| 分支预测失败多 | `perf stat -e branch-misses,branches <binary>` | miss 率 > 5% |
| 前端停顿 | `vtune -collect uarch-exploration <binary>` | Frontend Bound > 20% |
| 火焰图顶部有宽平台 | `./tools/performance/generate_flamegraph.sh <binary>` | 单函数 > 30% 采样 |

**典型原因**：计算密集循环未向量化、分支密集型逻辑、不必要的间接调用。

### 内存瓶颈

| 症状 | perf 命令 | 判断阈值 |
|------|-----------|----------|
| L1/L2 缓存 miss 高 | `perf stat -e cache-misses,cache-references <binary>` | miss 率 > 15% |
| 末级缓存 miss 高 | `perf stat -e LLC-load-misses,LLC-loads <binary>` | miss 率 > 10% |
| TLB miss 高 | `perf stat -e dTLB-load-misses,dTLB-loads <binary>` | miss 率 > 1% |
| 内存带宽饱和 | `vtune -collect memory-access <binary>` | 带宽接近理论峰值 |

**典型原因**：AOS 布局导致缓存行浪费、随机访问模式、工作集超过缓存容量。

### 并发问题

| 症状 | 检测方式 |
|------|----------|
| 多线程没有加速甚至变慢 | 对比单线程和多线程 benchmark 时间 |
| 偶发崩溃或数据损坏 | `cmake --preset=tsan && cmake --build build/tsan && ctest --preset=tsan` |
| CPU 利用率高但吞吐低 | `perf stat -e cache-misses <binary>` 看伪共享 |
| 锁等待时间长 | `perf record -g <binary> && perf report` 看 `pthread_mutex_lock` 占比 |

---

## 按瓶颈类型的优化路径

### CPU 密集 → 向量化 / 算法变更

**第一步：确认编译器是否已经向量化。**

```bash
# 查看向量化报告（Clang）
cmake --preset=release -DCMAKE_CXX_FLAGS="-Rpass=loop-vectorize -Rpass-missed=loop-vectorize"
cmake --build build/release 2>&1 | grep -i vectorize
```

**如果未向量化，尝试：**

| 手段 | 说明 | 示例位置 |
|------|------|----------|
| 加 `__restrict` | 告诉编译器指针不重叠 | `examples/04-simd-vectorization/src/` |
| 加 `alignas(64)` | 对齐数据，启用对齐加载 | `examples/02-memory-cache/src/` |
| 简化循环体 | 移除循环内的分支和函数调用 | — |
| 手动 intrinsics | 编译器搞不定时用 `_mm256_*` | `examples/04-simd-vectorization/src/` |

**如果已经向量化但还是慢，考虑算法变更：**

- O(n²) → O(n log n) 的算法替换永远比向量化有效
- 查表替代重复计算
- 减少不必要的精度（`double` → `float`）

详细原理见 [SIMD 向量化专题](../deep-dives/simd-internals.md)。

### 内存密集 → 布局优化 / 预取 / 分块

**第一步：确认是缓存问题。**

```bash
perf stat -e cache-misses,cache-references,LLC-load-misses \
  ./build/release/examples/02-memory-cache/aos_vs_soa_bench
```

**优化路径（按优先级）：**

| 优先级 | 手段 | 说明 | 仓库示例 |
|--------|------|------|----------|
| 1 | AOS → SOA | 把结构体数组变成数组结构体，让遍历只访问需要的字段 | `examples/02-memory-cache/bench/aos_soa_bench.cpp` |
| 2 | 数据对齐 | `alignas(64)` 避免跨缓存行 | `examples/02-memory-cache/bench/alignment_bench.cpp` |
| 3 | 软件预取 | `__builtin_prefetch` 提前加载下一批数据 | `examples/02-memory-cache/bench/prefetch_bench.cpp` |
| 4 | 分块（tiling） | 把大矩阵运算拆成缓存友好的小块 | — |

**AOS → SOA 示例：**

```cpp
// AOS：遍历 x 时，y 和 z 白白占用缓存行
struct Particle { float x, y, z; };
std::vector<Particle> particles(N);
for (auto& p : particles) { p.x += dx; }  // 每 12 字节只用 4 字节

// SOA：只加载需要的字段，缓存利用率 3x
struct Particles {
    std::vector<float> x, y, z;
};
for (size_t i = 0; i < N; ++i) { particles.x[i] += dx; }  // 连续访问
```

详细原理见 [内存布局专题](../deep-dives/memory-layout.md)。

### 并发 → 无锁 / 伪共享修复 / OpenMP

**第一步：确认是并发问题而非算法问题。**

```bash
# 单线程跑一次
./build/release/examples/05-concurrency/openmp_basics_bench \
  --benchmark_filter="single"

# 多线程跑一次
./build/release/examples/05-concurrency/openmp_basics_bench \
  --benchmark_filter="parallel"
```

如果多线程没有明显加速，问题在并发策略而非计算本身。

**优化路径：**

| 问题 | 手段 | 仓库示例 |
|------|------|----------|
| 锁竞争严重 | 缩小锁范围、换无锁结构 | `examples/05-concurrency/src/` |
| 伪共享 | `alignas(64)` 填充共享变量 | `examples/02-memory-cache/bench/false_sharing_bench.cpp` |
| 原子操作开销大 | 检查 memory order 是否可以放松 | `examples/05-concurrency/bench/atomic_bench.cpp` |
| 并行化不充分 | OpenMP 调整 schedule 和 chunk size | `examples/05-concurrency/bench/openmp_bench.cpp` |

**伪共享修复示例：**

```cpp
// 修复前：两个线程的计数器在同一缓存行，互相失效
struct Counters {
    std::atomic<int> a{0};  // 线程 1 写
    std::atomic<int> b{0};  // 线程 2 写 — 同一缓存行！
};

// 修复后：对齐到缓存行边界
struct Counters {
    alignas(64) std::atomic<int> a{0};
    alignas(64) std::atomic<int> b{0};
};
```

详细原理见 [无锁队列专题](../deep-dives/lock-free-queue.md)。

---

## 常见陷阱

### 陷阱 1：过早优化

```cpp
// 还没测量就手写 SIMD intrinsics
__m256 sum_vec = _mm256_setzero_ps();
for (int i = 0; i < n; i += 8) {
    __m256 v = _mm256_loadu_ps(&data[i]);
    sum_vec = _mm256_add_ps(sum_vec, v);
}
// 结果：瓶颈根本不在这里，是 I/O 等待
```

**正确做法**：先 `perf record` + 火焰图确认热点，再决定是否值得手动向量化。

### 陷阱 2：优化了错误的瓶颈

```cpp
// 花了一周把排序从 std::sort 换成手写 radix sort
// 但 perf 显示 80% 的时间花在数据反序列化上
```

**正确做法**：永远从火焰图最宽的平台开始。

### 陷阱 3：忽略算法复杂度

```cpp
// 用 SIMD 加速了一个 O(n²) 的循环
// 向量化带来 4x 加速，但换成 O(n log n) 算法能快 1000x
for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
        result[i] += matrix[i * n + j] * vec[j];  // O(n²)
```

**正确做法**：算法 > 数据结构 > 内存布局 > 向量化 > 微优化。按这个顺序检查。

### 陷阱 4：benchmark 被编译器优化掉

```cpp
static void BM_Broken(benchmark::State& state) {
    for (auto _ : state) {
        expensive_computation();  // 没有副作用，被编译器消除！
    }
}
```

**正确做法**：

```cpp
static void BM_Correct(benchmark::State& state) {
    for (auto _ : state) {
        auto result = expensive_computation();
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
}
```

---

## 代码组织最佳实践

### 命名空间

本仓库使用分层命名空间：

```cpp
namespace hpc::memory { /* 内存布局相关 */ }
namespace hpc::simd { /* SIMD 相关 */ }
namespace hpc::concurrent { /* 并发相关 */ }
namespace hpc::bench { /* benchmark 工具 */ }
```

### 头文件保护

使用 `#pragma once`（本仓库约定）：

```cpp
#pragma once

#include <cstddef>
#include <vector>

namespace hpc::memory {
// ...
}
```

### CMake 结构

每个模块遵循统一结构（见 [CMake 构建系统专题](../deep-dives/cmake-build-system.md)）：

```
examples/
  02-memory-cache/
    CMakeLists.txt      # 调用 hpc_add_example()
    include/            # 公共头文件
    src/                # 示例源码
    bench/              # benchmark 源码
```

benchmark 通过 `hpc_add_example()` 的 `BENCHMARK_SOURCES` 参数注册，自动链接 Google Benchmark 并注册为 CTest 测试。

### 格式化

```bash
# 格式化所有代码
./scripts/format.sh

# 只检查（CI 用）
./scripts/format.sh --check

# 只格式化 git 暂存区的文件
./scripts/format.sh --staged
```

---

## 决策速查表

| 情况 | 首先尝试 | 替代方案 |
|------|----------|----------|
| IPC < 0.5，火焰图有宽平台 | 检查是否可向量化（`-Rpass=loop-vectorize`） | 手动 intrinsics 或算法替换 |
| cache-miss > 15% | AOS → SOA 布局 | 数据对齐 + 预取 |
| LLC-miss > 10% | 分块（tiling）减小工作集 | 压缩数据结构 |
| branch-miss > 5% | 用分支无关代码（`cmov`、位运算） | 数据排序让分支可预测 |
| TLB-miss > 1% | 大页（`mmap` + `MAP_HUGETLB`） | 减少工作集、改善局部性 |
| 多线程无加速 | TSan 检查数据竞争 | 检查伪共享（`alignas(64)`） |
| 锁等待占比高 | 缩小锁范围 | 无锁数据结构 |
| `malloc` 在火焰图中很宽 | 预分配（`reserve`）或对象池 | 自定义分配器 |
| 不确定瓶颈在哪 | `perf stat` → `perf record` → 火焰图 | VTune `uarch-exploration` |

**通用原则**：算法复杂度 > 内存布局 > 向量化 > 微优化。从影响最大的层面开始。
