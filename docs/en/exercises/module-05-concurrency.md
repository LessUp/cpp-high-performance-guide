# Concurrency Exercises

Practice atomic operations, lock-free programming, and OpenMP parallelization.

---

## Exercise 1: Debug a Data Race

### Objective

Use ThreadSanitizer to find and fix data races.

### Setup

```cpp
// File: race_exercise.cpp
#include <thread>
#include <vector>
#include <iostream>

int counter = 0;  // BUG: Not atomic!

void increment(int times) {
    for (int i = 0; i < times; ++i) {
        counter++;  // Data race!
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
    
    std::cout << "Expected: " << threads * increments << "\n";
    std::cout << "Actual: " << counter << "\n";
    return 0;
}
```

### Tasks

1. **Compile and run**
   ```bash
   g++ -O2 -pthread race_exercise.cpp -o race_exercise
   ./race_exercise
   ```
   Note that the result is wrong!

2. **Detect with ThreadSanitizer**
   ```bash
   g++ -O2 -pthread -fsanitize=thread race_exercise.cpp -o race_exercise_tsan
   ./race_exercise_tsan
   ```

3. **Fix the data race**

4. **Verify the fix**

### Questions

<details>
<summary>Q: What are three ways to fix this?</summary>

1. Use `std::atomic<int>` with appropriate memory ordering
2. Use a mutex (`std::mutex`)
3. Use thread-local counters and combine at the end
</details>

---

## Exercise 2: Memory Ordering

### Objective

Understand the performance difference between memory orderings.

### Setup

```cpp
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

std::atomic<int> counter{0};

void increment_relaxed(int times) {
    for (int i = 0; i < times; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

void increment_seq_cst(int times) {
    for (int i = 0; i < times; ++i) {
        counter.fetch_add(1, std::memory_order_seq_cst);
    }
}
```

### Tasks

1. **Benchmark both versions**

2. **Compare performance**

3. **Explain the difference**

### Questions

<details>
<summary>Q: When should you use each ordering?</summary>

- `relaxed`: Counters, statistics where exact ordering doesn't matter
- `acquire/release`: Producer-consumer patterns
- `seq_cst`: Default, use when you're unsure or need total ordering
</details>

---

## Exercise 3: Implement a Thread-Safe Counter

### Objective

Implement a thread-safe counter with cache line padding.

### Setup

```cpp
// TODO: Implement this
class ThreadSafeCounter {
public:
    void increment();
    int get() const;
private:
    // Your members here
};
```

### Requirements

1. Support concurrent increments
2. Prevent false sharing between multiple counters
3. Support fast reads

### Tasks

1. **Implement the class**

2. **Test with multiple threads**

3. **Verify no false sharing**

### Hint

<details>
<summary>Click for hint</summary>

Use `alignas(64)` to prevent false sharing:

```cpp
struct alignas(64) CacheLineAligned {
    std::atomic<int> value{0};
};
```
</details>

---

## Exercise 4: SPSC Queue Verification

### Objective

Verify the correctness of a lock-free queue.

### Setup

Use the queue from `examples/05-concurrency/src/lock_free_queue.cpp`.

### Tasks

1. **Write a test** that:
   - Producer pushes N items
   - Consumer pops N items
   - Verify all items received

2. **Test under load**
   ```bash
   # Run with ThreadSanitizer
   cmake --preset=tsan
   cmake --build build/tsan
   ./build/tsan/your_test
   ```

3. **Verify no data races**

### Questions

<details>
<summary>Q: What invariants must the queue maintain?</summary>

1. No element is lost
2. No element is duplicated
3. FIFO ordering is preserved
4. Queue never enters invalid state (memory safety)
</details>

---

## Exercise 5: OpenMP Scaling

### Objective

Measure thread scaling efficiency with OpenMP.

### Setup

```cpp
#include <omp.h>
#include <vector>
#include <iostream>

void parallel_sum(const std::vector<int>& data) {
    long long sum = 0;
    
    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
    }
    
    std::cout << "Sum: " << sum << "\n";
}
```

### Tasks

1. **Measure with different thread counts**
   ```bash
   export OMP_NUM_THREADS=1
   time ./parallel_sum
   
   export OMP_NUM_THREADS=2
   time ./parallel_sum
   
   export OMP_NUM_THREADS=4
   time ./parallel_sum
   ```

2. **Calculate efficiency**
   - Efficiency = Actual Speedup / (Thread Count × 100%)
   - Ideal: 100% efficiency

3. **Identify bottlenecks**
   - Memory bandwidth?
   - Load imbalance?
   - False sharing?

### Questions

<details>
<summary>Q: Why does efficiency decrease with more threads?</summary>

Amdahl's Law: Serial portions don't parallelize
- Memory bandwidth saturation
- Synchronization overhead
- Cache contention
- NUMA effects on multi-socket systems
</details>

---

## Challenge: Parallelize a Real Algorithm

### Objective

Parallelize a matrix multiplication using OpenMP.

### Setup

```cpp
void matrix_mult_scalar(const float* A, const float* B, float* C,
                        size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}
```

### Tasks

1. **Parallelize with OpenMP**

2. **Optimize cache usage** (loop tiling)

3. **Combine with SIMD**

4. **Measure scaling efficiency**

### Hint

<details>
<summary>Click for hint</summary>

```cpp
void matrix_mult_openmp(const float* A, const float* B, float* C,
                        size_t N) {
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            float sum = 0.0f;
            #pragma omp simd reduction(+:sum)
            for (size_t k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}
```
</details>

---

## Solutions

See [solutions.md](solutions.md) for complete solutions.

---

## Next Steps

- Review the [Concurrency Module](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/05-concurrency)
- Read [Memory Utilities API](../reference/api/memory-utils.md) for cache line padding
- Explore [Profiling Guide](../guides/profiling-guide.md) for analyzing multi-threaded performance
