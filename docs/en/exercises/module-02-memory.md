# Memory Optimization Exercises

Practice cache optimization, data layout, and memory alignment techniques.

---

## Exercise 1: AOS to SOA Conversion

### Objective

Convert an Array of Structures layout to Structure of Arrays and measure the performance difference.

### Setup

The particle system example in `examples/02-memory-cache/src/aos_vs_soa.cpp` demonstrates this. You will modify it.

### Tasks

1. **Study the existing code**
   ```bash
   cat examples/02-memory-cache/src/aos_vs_soa.cpp
   ```

2. **Add a new field** to the particle system (e.g., `mass`)

3. **Update both AOS and SOA structures**

4. **Benchmark the change**
   ```bash
   ./build/release/examples/02-memory-cache/bench/aos_soa_bench
   ```

### Questions

<details>
<summary>Q1: How does adding a field affect AOS vs SOA performance?</summary>

Think about it before expanding...

Adding a field to AOS increases the stride between same-field accesses, making cache utilization worse for field-wise operations. SOA is unaffected because each field array is independent.
</details>

<details>
<summary>Q2: When would AOS be better than SOA?</summary>

AOS is better when you access all fields together (e.g., `particle.x`, `particle.y`, `particle.z` in sequence). This is common in object-oriented designs or when updating all particle properties at once.
</details>

---

## Exercise 2: Fix False Sharing

### Objective

Identify and fix false sharing in a multi-threaded counter.

### Setup

```cpp
// File: false_sharing_exercise.cpp
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

void increment_counters(std::atomic<int>* counters, int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counters[thread_id].fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    const int num_threads = 4;
    const int iterations = 10000000;
    
    // TODO: This has false sharing!
    std::atomic<int> counters[num_threads];
    
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        counters[i] = 0;
        threads.emplace_back(increment_counters, counters, i, iterations);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

### Tasks

1. **Compile and run** the code above
   ```bash
   g++ -O3 -pthread false_sharing_exercise.cpp -o false_sharing_exercise
   time ./false_sharing_exercise
   ```

2. **Fix the false sharing** using `alignas(64)` or padding

3. **Compare performance**

### Hint

<details>
<summary>Click for hint</summary>

The problem is that `std::atomic<int>` is typically 4 bytes, and 4 counters fit in a single cache line. Use alignment to ensure each counter is on its own cache line:

```cpp
struct alignas(64) AlignedCounter {
    std::atomic<int> value{0};
};

AlignedCounter counters[num_threads];
```
</details>

### Questions

<details>
<summary>Q: What speedup did you achieve?</summary>

Typical speedup is 2-10x depending on CPU and number of threads. More threads = more contention = bigger speedup from fixing false sharing.
</details>

---

## Exercise 3: Prefetch Distance Tuning

### Objective

Find the optimal prefetch distance for different access patterns.

### Setup

Use the prefetch example:
```bash
cat examples/02-memory-cache/src/prefetch.cpp
```

### Tasks

1. **Run the benchmark with default settings**
   ```bash
   ./build/release/examples/02-memory-cache/bench/prefetch_bench
   ```

2. **Modify the prefetch distance** (try 8, 16, 32, 64, 128)

3. **Plot the results**

### Questions

<details>
<summary>Q: What's the optimal prefetch distance?</summary>

The optimal distance depends on:
- Memory latency
- Loop iteration time
- Cache size

A good rule of thumb: `distance = memory_latency / iteration_time`. For typical systems, 32-64 elements ahead works well.
</details>

---

## Exercise 4: Memory Alignment for SIMD

### Objective

Measure the impact of memory alignment on SIMD performance.

### Tasks

1. **Create aligned and unaligned arrays**
   ```cpp
   // Unaligned
   float* unaligned = new float[1024];
   
   // Aligned
   alignas(64) float aligned[1024];
   // or
   float* aligned = static_cast<float*>(
       std::aligned_alloc(64, 1024 * sizeof(float))
   );
   ```

2. **Run SIMD operations on both**

3. **Compare performance**

### Questions

<details>
<summary>Q: When does alignment matter most?</summary>

Alignment matters most for:
- SIMD load/store instructions that require alignment (e.g., `_mm_load_ps` vs `_mm_loadu_ps`)
- AVX-512 which has stricter alignment requirements
- Large data sets where cache efficiency is critical
</details>

---

## Challenge: Profile a Mystery Program

### Objective

Use profiling tools to identify and fix a performance bottleneck.

### Setup

A "mystery" program with a hidden bottleneck is provided in the exercises directory.

### Tasks

1. **Profile the program**
   ```bash
   perf record -g ./mystery_program
   perf report
   ```

2. **Generate a FlameGraph**
   ```bash
   perf script | stackcollapse-perf.pl | flamegraph.pl > mystery_flame.svg
   ```

3. **Identify the bottleneck**

4. **Fix it**

5. **Measure the improvement**

### Hint

<details>
<summary>Click for hint</summary>

Look for:
- Wide sections in the FlameGraph (hot functions)
- High cache miss rates
- Functions taking unexpectedly long
</details>

---

## Solutions

See [solutions.md](solutions.md) for complete solutions to these exercises.

---

## Next Steps

- Continue to [SIMD Exercises](module-04-simd.md)
- Read the [Memory Utilities API](../reference/api/memory-utils.md)
- Explore [Optimization Decision Tree](../guides/optimization-decision-tree.md)
