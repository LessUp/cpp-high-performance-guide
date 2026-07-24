---
type: deep-dive
difficulty: advanced
description: 深入探讨无锁队列的实现，涵盖 SPSC、MPMC 模式与内存序考量。
---

# 无锁队列深度专题

本深度专题探讨无锁并发队列背后的实现与理论，它是高性能并发系统的基础构件。

## 为什么需要无锁？

### 基于锁的问题

传统的基于 mutex 的队列存在若干问题：

```cpp
// Lock-based queue - common issues
template<typename T>
class LockedQueue {
    std::queue<T> queue_;
    std::mutex mutex_;

public:
    void push(T value) {
        std::lock_guard lock(mutex_);  // Blocks all other operations
        queue_.push(std::move(value));
    }

    bool pop(T& value) {
        std::lock_guard lock(mutex_);  // Blocks all operations
        if (queue_.empty()) return false;
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }
};
```

**问题：**
1. **竞争（Contention）**：所有线程在 mutex 上串行化
2. **优先级反转（Priority inversion）**：高优先级线程被低优先级线程阻塞
3. **死锁风险（Deadlock potential）**：复杂系统中的锁顺序问题
4. **护送效应（Convoy effect）**：一个慢线程阻塞所有其他线程

### 无锁的收益

| 属性 | 基于锁 | 无锁 |
|----------|------------|-----------|
| 推进保证 | 被阻塞的线程可能阻塞全部 | 单个线程失败不会阻塞其他线程 |
| 优先级反转 | 可能发生 | 不会发生 |
| 死锁 | 可能发生 | 不会发生 |
| 竞争下的吞吐 | 退化 | 维持 |

## 内存序基础

C++ 内存模型定义了六种排序约束：

```cpp
namespace std {
    enum memory_order {
        relaxed,   // No ordering guarantees
        consume,   // Data dependency ordering (rarely used)
        acquire,   // Prevents reordering of reads before
        release,   // Prevents reordering of writes after
        acq_rel,   // Combined acquire + release
        seq_cst    // Sequential consistency (default, strongest)
    };
}
```

### Acquire-Release 语义

```
Thread A (Producer)          Thread B (Consumer)
─────────────────           ─────────────────
data = 42;
ready.store(true,           while (!ready.load(
    memory_order_release);      memory_order_acquire));
                            // Guaranteed to see data = 42
                            use(data);
```

**关键洞察**：写端的 `release` 与读端的 `acquire` 同步（synchronizes-with），从而建立起 happens-before 关系。

### 排序强度与性能

```cpp
// Relaxed - fastest, no synchronization
counter.fetch_add(1, std::memory_order_relaxed);

// Acquire-Release - producer-consumer synchronization
flag.store(true, std::memory_order_release);
if (flag.load(std::memory_order_acquire)) { /* ... */ }

// Sequential consistency - strongest guarantee, slowest
x.store(1, std::memory_order_seq_cst);
```

**基准测试**（对计数器自增 1 亿次）：

| 排序 | 时间 (ms) | 相对开销 |
|----------|-----------|---------------|
| relaxed | 280 | 1.0x |
| acquire | 320 | 1.14x |
| seq_cst | 450 | 1.61x |

## SPSC 队列实现

单生产者单消费者（Single-Producer Single-Consumer）队列是最简单、最快的无锁结构。

### 环形缓冲区设计

```cpp
template<typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be power of 2");

    alignas(64) std::atomic<size_t> head_{0};  // Consumer index
    alignas(64) std::atomic<size_t> tail_{0};  // Producer index
    std::array<T, Capacity> buffer_;

    static constexpr size_t MASK = Capacity - 1;

public:
    bool try_push(const T& value) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (tail + 1) & MASK;

        // Check if full
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;  // Queue full
        }

        buffer_[tail] = value;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool try_pop(T& value) {
        const size_t head = head_.load(std::memory_order_relaxed);

        // Check if empty
        if (head == tail_.load(std::memory_order_acquire)) {
            return false;  // Queue empty
        }

        value = buffer_[head];
        head_.store((head + 1) & MASK, std::memory_order_release);
        return true;
    }
};
```

### 关键设计要点

1. **cache line 分离**：`head_` 与 `tail_` 位于不同的 cache line，避免生产者与消费者之间的 false sharing
2. **容量为 2 的幂**：用按位与（`& MASK`）替代取模（`% Capacity`）
3. **本地访问用 relaxed**：每个线程对自身索引使用 relaxed 排序
4. **同步用 acquire-release**：跨线程的索引读取使用恰当的排序

### SPSC 性能

| 操作 | 延迟 (ns) | 吞吐 (M/s) |
|-----------|--------------|------------------|
| try_push | ~3 | ~300 |
| try_pop | ~3 | ~300 |
| 完整往返 | ~8 | ~125 |

## MPMC 队列实现

多生产者多消费者（Multiple-Producer Multiple-Consumer）队列更复杂，但支持通用的并发场景。

### 序列号方法

```cpp
template<typename T, size_t Capacity>
class MPMCQueue {
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    alignas(64) std::atomic<size_t> enqueue_pos_{0};
    alignas(64) std::atomic<size_t> dequeue_pos_{0};
    std::array<Cell, Capacity> buffer_;

public:
    MPMCQueue() {
        for (size_t i = 0; i < Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    bool try_enqueue(const T& value) {
        Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);

        for (;;) {
            cell = &buffer_[pos % Capacity];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (diff == 0) {
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1,
                        std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false;  // Queue full
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }

        cell->data = value;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool try_dequeue(T& value) {
        Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

        for (;;) {
            cell = &buffer_[pos % Capacity];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) -
                           static_cast<intptr_t>(pos + 1);

            if (diff == 0) {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                        std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false;  // Queue empty
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }

        value = std::move(cell->data);
        cell->sequence.store(pos + Capacity, std::memory_order_release);
        return true;
    }
};
```

### MPMC 复杂度分析

| 指标 | SPSC | MPMC |
|--------|------|------|
| 竞争点 | 0（索引分离） | 2（共享入队/出队位置） |
| CAS 操作 | 0 | 每次操作 1-2 次 |
| 内存序 | acquire-release | acquire-release + CAS |
| 扩展性 | 线性 | 随竞争退化 |

## 选择内存序

### 决策树

```
Is the data self-contained (no dependencies)?
├── Yes → relaxed is sufficient
│
└── No → Is there a producer-consumer relationship?
    ├── Yes → acquire-release
    │
    └── No → Is global ordering important?
        ├── Yes → seq_cst
        └── No → acquire-release
```

### 按用例举例

```cpp
// Counter (no synchronization needed)
stats.counter.fetch_add(1, std::memory_order_relaxed);

// Flag to signal completion
done.store(true, std::memory_order_release);
// ... other thread ...
while (!done.load(std::memory_order_acquire));

// Multiple flags needing total order
std::atomic<bool> flag1{false}, flag2{false};
flag1.store(true, std::memory_order_seq_cst);
flag2.store(true, std::memory_order_seq_cst);
// All threads see same ordering
```

## 实践指南

### 何时使用每种模式

| 场景 | 队列类型 | 排序 |
|----------|------------|----------|
| 单生产者，单消费者 | SPSC | acquire-release |
| 日志线程 + 工作线程 | MPSC | acquire-release |
| 线程池任务队列 | MPMC | acquire-release + CAS |
| 事件计数器 | N/A | relaxed |
| 停止标志 | N/A | acquire-release |

### 常见陷阱

1. **排序错误**：在需要同步时使用了 `relaxed`
2. **false sharing**：将 atomic 变量放在同一 cache line 上
3. **ABA 问题**：在 CAS 循环中使用指针而未做版本化
4. **内存回收**：在所有线程完成之前复用内存

## 参考文献

<Citation
  :references="[
    {
      id: 'williams2019',
      author: 'Williams, A.',
      title: 'C++ Concurrency in Action',
      year: 2019,
      source: 'Manning Publications',
      url: 'https://www.manning.com/books/c-plus-plus-concurrency-in-action-second-edition'
    },
    {
      id: 'herlihy2012',
      author: 'Herlihy, M. & Shavit, N.',
      title: 'The Art of Multiprocessor Programming',
      year: 2012,
      source: 'Morgan Kaufmann',
      url: 'https://www.elsevier.com/books/the-art-of-multiprocessor-programming/herlihy/978-0-12-370591-4'
    },
    {
      id: 'dvyukov2010',
      author: 'Dvyukov, D.',
      title: 'Bounded MPMC queue',
      year: 2010,
      source: '1024cores.net',
      url: 'https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue'
    }
  ]"
/>
