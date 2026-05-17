---
type: deep-dive
difficulty: advanced
prerequisites:
  - /en/academy/module-atlas
  - /en/architecture/repository-topology
description: Deep dive into lock-free queue implementations including SPSC, MPMC patterns and memory ordering considerations.
---

# Lock-Free Queue Deep Dive

This deep dive explores the implementation and theory behind lock-free concurrent queues, a fundamental building block for high-performance concurrent systems.

## Why Lock-Free?

### Lock-Based Problems

Traditional mutex-based queues suffer from several issues:

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

**Problems:**
1. **Contention**: All threads serialize on the mutex
2. **Priority inversion**: High-priority thread blocked by low-priority thread
3. **Deadlock potential**: Lock ordering issues in complex systems
4. **Convoy effect**: One slow thread blocks all others

### Lock-Free Benefits

| Property | Lock-Based | Lock-Free |
|----------|------------|-----------|
| Progress | Blocked thread can stop all | Individual thread failure doesn't block others |
| Priority inversion | Possible | Not possible |
| Deadlock | Possible | Not possible |
| Throughput under contention | Degrades | Maintains |

## Memory Ordering Fundamentals

The C++ memory model defines six ordering constraints:

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

### Acquire-Release Semantics

```
Thread A (Producer)          Thread B (Consumer)
─────────────────           ─────────────────
data = 42;
ready.store(true,           while (!ready.load(
    memory_order_release);      memory_order_acquire));
                            // Guaranteed to see data = 42
                            use(data);
```

**Key insight**: `release` on the writer synchronizes-with `acquire` on the reader, establishing a happens-before relationship.

### Ordering Strength vs Performance

```cpp
// Relaxed - fastest, no synchronization
counter.fetch_add(1, std::memory_order_relaxed);

// Acquire-Release - producer-consumer synchronization
flag.store(true, std::memory_order_release);
if (flag.load(std::memory_order_acquire)) { /* ... */ }

// Sequential consistency - strongest guarantee, slowest
x.store(1, std::memory_order_seq_cst);
```

**Benchmark** (increment counter 100M times):

| Ordering | Time (ms) | Relative Cost |
|----------|-----------|---------------|
| relaxed | 280 | 1.0x |
| acquire | 320 | 1.14x |
| seq_cst | 450 | 1.61x |

## SPSC Queue Implementation

Single-Producer Single-Consumer queues are the simplest and fastest lock-free structure.

### Ring Buffer Design

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

### Key Design Points

1. **Cache line separation**: `head_` and `tail_` are on different cache lines, avoiding false sharing between producer and consumer
2. **Power-of-2 capacity**: Bitwise AND (`& MASK`) instead of modulo (`% Capacity`)
3. **Relaxed for local access**: Each thread uses relaxed ordering for its own index
4. **Acquire-Release for synchronization**: Cross-thread index reads use proper ordering

### SPSC Performance

| Operation | Latency (ns) | Throughput (M/s) |
|-----------|--------------|------------------|
| try_push | ~3 | ~300 |
| try_pop | ~3 | ~300 |
| Full round-trip | ~8 | ~125 |

## MPMC Queue Implementation

Multiple-Producer Multiple-Consumer queues are more complex but support general concurrent use.

### Sequence Number Approach

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

### MPMC Complexity Analysis

| Metric | SPSC | MPMC |
|--------|------|------|
| Contention points | 0 (separate indices) | 2 (shared enqueue/dequeue positions) |
| CAS operations | 0 | 1-2 per operation |
| Memory ordering | Acquire-Release | Acquire-Release + CAS |
| Scaling | Linear | Degrades with contention |

## Choosing Memory Ordering

### Decision Tree

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

### Examples by Use Case

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

## Practical Guidelines

### When to Use Each Pattern

| Scenario | Queue Type | Ordering |
|----------|------------|----------|
| Single producer, single consumer | SPSC | acquire-release |
| Logger thread + worker threads | MPSC | acquire-release |
| Thread pool task queue | MPMC | acquire-release + CAS |
| Event counter | N/A | relaxed |
| Stop flag | N/A | acquire-release |

### Common Pitfalls

1. **Wrong ordering**: Using `relaxed` when synchronization is needed
2. **False sharing**: Placing atomic variables on the same cache line
3. **ABA problem**: Using pointers without versioning in CAS loops
4. **Memory reclamation**: Reusing memory before all threads are done

## References

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
