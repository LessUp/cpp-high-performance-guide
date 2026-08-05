# 无锁并发

## 为什么需要无锁

互斥锁（mutex）是并发编程中最常用的同步原语，但在高性能场景下它有三个根本性问题：

**竞争与上下文切换。** 当多个线程争抢同一把锁时，失败的线程被挂起（futex wait），触发内核态切换。在临界区极短的场景下（如原子计数器递增），锁的开销远超临界区本身。

**优先级反转。** 低优先级线程持锁时，高优先级线程被阻塞。如果中间优先级线程抢先运行，高优先级线程的等待时间变得不可预测。这在实时系统中是致命的。

**护送效应（convoying）。** 锁释放后，所有等待线程被唤醒并再次竞争。大量线程同时争抢导致 cache line 在核间反复弹跳（cache line bouncing），吞吐量随线程数增加反而下降。

无锁（lock-free）数据结构通过原子操作（CAS、fetch_add）实现线程间协调，保证系统整体始终有线程能取得进展。本仓库的 `examples/05-concurrency/` 模块提供了从内存序基础到完整 SPSC/MPMC 队列的渐进式实现。

---

## 内存序

`std::atomic` 的操作并非只有"原子"这一个维度——memory order 决定了该操作对其它线程的可见性约束。从弱到强共 6 种：

| Memory Order | 语义 | 典型用途 |
|---|---|---|
| `relaxed` | 仅保证原子性，无同步 | 统计计数器 |
| `consume` | 数据依赖同步（已弃用） | 避免使用 |
| `acquire` | 后续读不可重排到此操作之前 | 读取共享标志 |
| `release` | 先前写不可重排到此操作之后 | 发布共享数据 |
| `acq_rel` | 兼具 acquire + release | RMW 操作（fetch_add） |
| `seq_cst` | 全局顺序一致（默认） | 需要全局排序的算法 |

### Acquire-Release 语义

核心模式：生产者用 release 发布数据，消费者用 acquire 读取数据。release 之前的所有写操作对 acquire 之后的读操作可见。

```
Thread A (Producer)              Thread B (Consumer)
─────────────────                ─────────────────
data = 42  (relaxed)
ready = true  (release)  ───►   while (!ready.load(acquire)) {}
                                 assert(data == 42)  // 保证成立
```

仓库中的实现（`examples/05-concurrency/src/atomic_ordering.cpp`）：

```cpp
void demonstrate_acquire_release() {
    std::atomic<int> data{0};
    std::atomic<bool> ready{false};

    // Producer thread
    std::thread producer([&]() {
        data.store(42, std::memory_order_relaxed);     // Write data
        ready.store(true, std::memory_order_release);  // Release: make data visible
    });

    // Consumer thread
    std::thread consumer([&]() {
        while (!ready.load(std::memory_order_acquire)) {  // Acquire: wait for data
            // Spin
        }
        int value = data.load(std::memory_order_relaxed);
        assert(value == 42);  // Guaranteed by acquire-release pair
    });

    producer.join();
    consumer.join();
}
```

关键点：`data` 的 store 使用 relaxed 即可——同步由 `ready` 的 release/acquire 对保证。这比两个操作都用 seq_cst 更轻量。

### Sequential Consistency 与 Store Buffering

seq_cst 提供全局单一修改顺序。经典的 store buffering litmus test：

```cpp
// 两个线程分别执行：
// T1: x.store(1, seq_cst); r1 = y.load(seq_cst);
// T2: y.store(1, seq_cst); r2 = x.load(seq_cst);
// seq_cst 保证：不可能同时 r1==0 && r2==0
```

仓库中用 10000 次试验验证这一不变量（`demonstrate_sequential_consistency()`）。

### 性能差异

在 x86 上，由于 TSO（Total Store Order）内存模型本身较强，relaxed 与 seq_cst 的 `fetch_add` 性能差异很小。在 ARM/POWER 等弱内存模型架构上差异显著。

运行性能对比：

```bash
./build/release/examples/05-concurrency/atomic_ordering_bench
```

或直接运行 demo 观察三种 ordering 的计时：

```bash
./build/release/examples/05-concurrency/atomic_ordering
```

---

## SPSC 队列

单生产者-单消费者（SPSC）队列是无锁数据结构中最实用的形态：无需 CAS 循环，仅靠 acquire-release 对即可实现正确同步。

### 环形缓冲区设计

`include/hpc/lock_free_queue.hpp` 中的 `SPSCQueue<T, Capacity>`：

```cpp
template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

    // ...
private:
    static constexpr size_t MASK = Capacity - 1;

    alignas(hpc::core::CACHE_LINE_SIZE) std::atomic<size_t> head_;
    alignas(hpc::core::CACHE_LINE_SIZE) std::atomic<size_t> tail_;
    alignas(hpc::core::CACHE_LINE_SIZE) std::optional<T> buffer_[Capacity];
};
```

### 关键设计要点

**1. 2 的幂容量 → 位运算取模**

```cpp
const size_t next_tail = (current_tail + 1) & MASK;  // 等价于 % Capacity
```

位与运算（1 cycle）替代除法/取模指令（20-40 cycles），在热路径上差异显著。

**2. Cache line 分离 → 消除 false sharing**

`head_` 和 `tail_` 分别对齐到独立 cache line（通常 64 字节）。生产者只写 `tail_`、读 `head_`；消费者只写 `head_`、读 `tail_`。如果两者共享 cache line，每次写入都会使对方核心的缓存行失效，导致严重的性能退化。

**3. Relaxed vs Acquire-Release 的精确使用**

```cpp
bool push(const T& value) {
    const size_t current_tail = tail_.load(std::memory_order_relaxed);  // 只有生产者写 tail_
    const size_t next_tail = (current_tail + 1) & MASK;

    if (next_tail == head_.load(std::memory_order_acquire)) {  // 同步消费者的 head_ 更新
        return false;  // 队列满
    }

    buffer_[current_tail].emplace(value);
    tail_.store(next_tail, std::memory_order_release);  // 发布数据给消费者
    return true;
}

std::optional<T> pop() {
    const size_t current_head = head_.load(std::memory_order_relaxed);  // 只有消费者写 head_

    if (current_head == tail_.load(std::memory_order_acquire)) {  // 同步生产者的 tail_ 更新
        return std::nullopt;  // 队列空
    }

    T value = std::move(*buffer_[current_head]);
    buffer_[current_head].reset();
    head_.store((current_head + 1) & MASK, std::memory_order_release);  // 发布空位给生产者
    return value;
}
```

设计逻辑：
- 读自己的索引（`tail_` 对生产者，`head_` 对消费者）用 relaxed——单写者无需同步。
- 读对方的索引用 acquire——确保看到对方最新发布的状态。
- 写自己的索引用 release——确保 buffer 写入对对方可见。

**4. 一个槽位永远为空**

`capacity()` 返回 `Capacity - 1`。用空槽区分"满"和"空"，避免引入额外的计数器或标志位。

### MPMC 扩展

同文件中的 `MPMCQueue` 使用 per-cell sequence number + CAS 实现多生产者多消费者安全。核心思路：每个 cell 维护一个原子 sequence，生产者/消费者通过 CAS 竞争 `enqueue_pos_`/`dequeue_pos_`，用 sequence 差值判断 cell 状态。

### 运行验证

```bash
# 构建
cmake --preset=release && cmake --build build/release

# 运行 SPSC/MPMC demo
./build/release/examples/05-concurrency/lock_free_queue
```

输出验证 FIFO 顺序保持和元素完整性。

---

## OpenMP 并行

对于数据并行（data parallelism）场景，OpenMP 提供了比手动线程管理更简洁的编程模型。`examples/05-concurrency/src/openmp_basics.cpp` 覆盖了核心模式。

### Parallel For

```cpp
#pragma omp parallel for
for (size_t i = 0; i < N; ++i) {
    data[i] = std::sin(static_cast<double>(i) * 0.001);
}
```

每个迭代独立，编译器自动将迭代空间切分给线程池中的线程。

### Reduction

```cpp
double par_sum = 0.0;
#pragma omp parallel for reduction(+ : par_sum)
for (size_t i = 0; i < N; ++i) {
    par_sum += data[i];
}
```

OpenMP 为每个线程维护私有副本，循环结束后合并。无需手动同步。

### 调度策略

对于负载不均匀的循环：

```cpp
#pragma omp parallel for schedule(dynamic, 1000)  // 动态分配 chunk
#pragma omp parallel for schedule(guided)          // 递减 chunk 大小
```

- `static`：编译时固定分配，零开销但负载不均时效率低
- `dynamic`：运行时按需领取，适合不规则负载
- `guided`：chunk 从大到小递减，兼顾调度开销和负载均衡

### 线程扩展效率

```bash
./build/release/examples/05-concurrency/openmp_basics_bench
```

或运行 demo 观察不同线程数下的加速比：

```bash
./build/release/examples/05-concurrency/openmp_basics
```

理想情况下 N 线程获得 N 倍加速，但受限于：
- 内存带宽饱和（compute-bound → memory-bound 转变）
- NUMA 效应（跨 socket 访问延迟）
- 线程创建/同步开销

---

## 验证

并发代码的正确性极难通过测试穷举验证。ThreadSanitizer（TSan）通过 happens-before 关系追踪检测数据竞争。

### 运行 TSan

```bash
# 配置（使用 Clang，TSan 在 Clang 上支持更完善）
cmake --preset=tsan

# 构建
cmake --build build/tsan

# 运行测试
ctest --preset=tsan
```

TSan preset 的关键配置（`CMakePresets.json`）：

```json
{
  "name": "tsan",
  "inherits": "base",
  "environment": {
    "CC": "clang",
    "CXX": "clang++"
  },
  "cacheVariables": {
    "CMAKE_BUILD_TYPE": "Debug",
    "ENABLE_TSAN": "ON",
    "HPC_BUILD_TESTS": "ON",
    "HPC_BUILD_BENCHMARKS": "OFF"
  }
}
```

注意：
- TSan 与 ASan 互斥，不可同时启用
- TSan 需要 Debug 构建（优化会消除某些竞争窗口）
- 测试时设置 `TSAN_OPTIONS=halt_on_error=1` 在首个错误时停止

### 相关测试文件

- `tests/unit/concurrency/lock_free_queue_test.cpp` — SPSC/MPMC 队列单元测试
- `tests/unit/concurrency/concurrency_utils_test.cpp` — 并发工具测试
- `tests/property/concurrency_properties.cpp` — 基于 RapidCheck 的属性测试

属性测试对并发代码尤其有价值：它生成大量随机输入序列，验证不变量（如 FIFO 顺序、元素不丢失）在任意交错下成立。

---

## 参考文献

- Herlihy, M. & Wing, J. "Linearizability: A Correctness Condition for Concurrent Objects" (1990)
- Vyukov, D. "Bounded MPMC queue" — 1024cores.net 上的 sequence-based MPMC 设计
- C++ Standard [atomics.order] — memory_order 的形式化定义
- Sutter, H. "The Free Lunch Is Over" — 并发编程的必要性论证
- Williams, A. "C++ Concurrency in Action" (2nd ed.) — acquire-release 语义的工程实践
- OpenMP 5.2 Specification — schedule 子句和 reduction 的语义定义
