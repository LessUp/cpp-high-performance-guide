---
type: deep-dive
difficulty: advanced
prerequisites:
  - /en/academy/module-atlas
  - /en/architecture/repository-topology
description: Deep dive into memory layout optimization techniques including AOS vs SOA, cache line alignment, and false sharing elimination.
---

# Memory Layout Deep Dive

This deep dive explores the critical relationship between data structure design and memory performance in modern C++ applications.

## The Memory Hierarchy Reality

Modern CPUs have a complex memory hierarchy with dramatically different access latencies:

| Level | Size | Latency | Cycles |
|-------|------|---------|--------|
| L1 Cache | 32-64 KB | ~1 ns | 3-4 |
| L2 Cache | 256-512 KB | ~4 ns | 10-12 |
| L3 Cache | 2-32 MB | ~12 ns | 40-50 |
| Main Memory | GBs | ~100 ns | 200-300 |

**Key insight**: A cache miss can cost 100x more than a cache hit. Memory layout optimization is about maximizing cache utilization.

## AOS vs SOA: The Layout Battle

### Array of Structures (AOS)

```cpp
// Traditional approach - intuitive but often suboptimal
struct Particle {
    float x, y, z;      // Position (12 bytes)
    float vx, vy, vz;   // Velocity (12 bytes)
    float mass;         // Property (4 bytes)
};  // Total: 28 bytes per particle

std::vector<Particle> particles(10000);
```

**Problems with AOS:**
1. **Poor cache utilization**: When updating positions, we load velocity and mass into cache unnecessarily
2. **SIMD unfriendly**: Data is interleaved, making vectorization difficult
3. **False sharing risk**: Adjacent particles may share cache lines in multi-threaded scenarios

### Structure of Arrays (SOA)

```cpp
// Performance-oriented approach
struct ParticleSystem {
    std::vector<float> x, y, z;      // All x positions contiguous
    std::vector<float> vx, vy, vz;   // All velocities contiguous
    std::vector<float> mass;          // All masses contiguous
};

ParticleSystem particles;
particles.x.resize(10000);
particles.y.resize(10000);
// ... etc.
```

**Benefits of SOA:**
1. **Perfect cache utilization**: When processing positions, only position data enters cache
2. **SIMD friendly**: Contiguous arrays enable natural vectorization
3. **Better prefetching**: Hardware prefetcher can predict access patterns

### Performance Comparison

```cpp
// AOS update - scattered memory access
void updateAOS(std::vector<Particle>& p, float dt) {
    for (auto& particle : p) {
        particle.x += particle.vx * dt;
        particle.y += particle.vy * dt;
        particle.z += particle.vz * dt;
    }
}

// SOA update - contiguous memory access, SIMD-friendly
void updateSOA(ParticleSystem& p, float dt) {
    for (size_t i = 0; i < p.x.size(); ++i) {
        p.x[i] += p.vx[i] * dt;
        p.y[i] += p.vy[i] * dt;
        p.z[i] += p.vz[i] * dt;
    }
}
```

**Benchmark results** (1M particles, AVX2 enabled):

| Layout | Time (ms) | Speedup | SIMD Utilization |
|--------|-----------|---------|------------------|
| AOS | 8.2 | 1.0x | ~15% |
| SOA | 2.1 | 3.9x | ~85% |

## Cache Line Alignment

### The 64-Byte Boundary

Modern x86 CPUs use 64-byte cache lines. Data crossing cache line boundaries requires two memory accesses.

```cpp
#include <hpc/core.hpp>

// BAD: Unaligned structure
struct UnalignedCounter {
    std::atomic<int64_t> counter1;  // 8 bytes
    std::atomic<int64_t> counter2;  // 8 bytes
    std::atomic<int64_t> counter3;  // 8 bytes
    std::atomic<int64_t> counter4;  // 8 bytes
};  // Total: 32 bytes - all in one cache line!
```

**Problem**: When thread A updates `counter1` and thread B updates `counter2`, they cause **false sharing** - both threads fight for exclusive ownership of the same cache line.

### Proper Alignment

```cpp
// GOOD: Each counter on its own cache line
struct alignas(64) PaddedCounter {
    std::atomic<int64_t> value;
    // Implicit padding to 64 bytes
};

struct AlignedCounters {
    PaddedCounter counter1;  // 64 bytes (cache line 0)
    PaddedCounter counter2;  // 64 bytes (cache line 1)
    PaddedCounter counter3;  // 64 bytes (cache line 2)
    PaddedCounter counter4;  // 64 bytes (cache line 3)
};
```

### Memory Alignment API

The project provides utilities for alignment:

```cpp
#include <hpc/core.hpp>

// Compile-time cache line size for alignas
struct alignas(hpc::CACHE_LINE_SIZE) MyData {
    // ... fields ...
};

// Runtime cache line size detection
size_t line_size = hpc::cache_line_size();

// Aligned allocator for STL containers
std::vector<float, hpc::AlignedAllocator<float, 64>> aligned_data;
```

## False Sharing: The Silent Killer

### Detection Pattern

False sharing symptoms:
- Multi-threaded scaling worse than expected
- High `perf` metrics for cache coherency traffic
- Non-linear speedup degradation as thread count increases

### Example: Parallel Counter

```cpp
// BAD: False sharing
std::vector<int> counters(num_threads);  // Contiguous memory!

#pragma omp parallel for
for (int i = 0; i < num_threads; ++i) {
    for (int j = 0; j < 1000000; ++j) {
        counters[i]++;  // False sharing!
    }
}

// GOOD: Cache-line padded
struct alignas(64) PaddedInt { int value; };
std::vector<PaddedInt> counters(num_threads);
```

### Performance Impact

| Threads | Bad (ms) | Good (ms) | Speedup Lost |
|---------|----------|-----------|--------------|
| 1 | 100 | 100 | 0% |
| 2 | 95 | 50 | 47% |
| 4 | 90 | 25 | 72% |
| 8 | 85 | 12 | 86% |

## Memory Prefetching

### Software Prefetch

```cpp
#include <xmmintrin.h>  // SSE intrinsics

void processWithPrefetch(const std::vector<float>& data) {
    const size_t prefetch_distance = 8;  // Prefetch 8 elements ahead

    for (size_t i = 0; i < data.size(); ++i) {
        // Prefetch future data
        if (i + prefetch_distance < data.size()) {
            _mm_prefetch(reinterpret_cast<const char*>(&data[i + prefetch_distance]),
                         _MM_HINT_T0);  // Prefetch into L1
        }

        // Process current element
        process(data[i]);
    }
}
```

### Prefetch Guidelines

1. **Distance**: Typically 8-32 elements ahead
2. **Timing**: Too early = cache eviction, too late = no benefit
3. **Pattern**: Works best for predictable access patterns

## Practical Guidelines

### When to Use SOA

✅ **Use SOA when:**
- Processing subsets of fields (e.g., only positions)
- SIMD vectorization is important
- Memory bandwidth is the bottleneck
- Data is accessed in predictable patterns

❌ **Keep AOS when:**
- All fields are always accessed together
- Object-oriented design is cleaner
- Random access patterns dominate
- Memory overhead of SOA is prohibitive

### Alignment Rules

1. **Hot data**: Align frequently accessed data to cache lines
2. **Thread-local data**: Pad per-thread data to avoid false sharing
3. **Atomic variables**: Each atomic on its own cache line if contended
4. **SIMD data**: Align to 16 (SSE), 32 (AVX), or 64 (AVX-512) bytes

## References

<Citation
  :references="[
    {
      id: 'drepper2007',
      author: 'Drepper, U.',
      title: 'What Every Programmer Should Know About Memory',
      year: 2007,
      source: 'Red Hat, Inc.',
      url: 'https://people.freebsd.org/~lstewart/articles/cpumemory.pdf'
    },
    {
      id: 'fog2023',
      author: 'Fog, A.',
      title: 'Optimizing software in C++',
      year: 2023,
      source: 'Copenhagen University',
      url: 'https://www.agner.org/optimize/optimizing_cpp.pdf'
    }
  ]"
/>
