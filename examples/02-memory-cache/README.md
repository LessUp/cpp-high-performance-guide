# 02 - Memory & Cache Optimization

<p align="center">
  <img src="https://img.shields.io/badge/Language-C%2B%2B20-blue.svg" alt="C++20">
  <img src="https://img.shields.io/badge/Difficulty-Beginner-green.svg" alt="Difficulty">
  <img src="https://img.shields.io/badge/Topic-Memory-orange.svg" alt="Topic">
</p>

> Master memory and cache optimization techniques critical for high-performance computing.

Learn how data layout, alignment, and prefetching can deliver **2-20x performance improvements**.

---

## Memory Layout Visualization

### AOS vs SOA Memory Layout

```mermaid
graph TB
    subgraph AOS["Array of Structures (AOS)"]
        direction LR
        P1["Particle 0<br/>x₀ y₀ z₀ vx₀ vy₀ vz₀"]
        P2["Particle 1<br/>x₁ y₁ z₁ vx₁ vy₁ vz₁"]
        P3["Particle 2<br/>x₂ y₂ z₂ vx₂ vy₂ vz₂"]
    end
    
    subgraph SOA["Structure of Arrays (SOA)"]
        direction LR
        X["x: x₀ x₁ x₂ ..."]
        Y["y: y₀ y₁ y₂ ..."]
        Z["z: z₀ z₁ z₂ ..."]
        VX["vx: vx₀ vx₁ vx₂ ..."]
    end
    
    subgraph CacheLine["Cache Line (64 bytes)"]
        C1["16 floats of same field"]
    end
    
    AOS -->|"Poor cache utilization"| Note1["❌ Only 2-3 particles<br/>per cache line when<br/>accessing single field"]
    SOA -->|"Excellent cache utilization"| Note2["✓ 16 values per cache line<br/>when processing one field"]
    
    style AOS fill:#ffcccc
    style SOA fill:#ccffcc
    style CacheLine fill:#e6f3ff
```

**When updating all x coordinates:**
- AOS: Loads unnecessary y, z, vx, vy, vz data → Poor cache utilization
- SOA: Only loads x values → Maximum cache efficiency

---

## False Sharing Visualization

```mermaid
graph TB
    subgraph Bad["❌ False Sharing"]
        direction LR
        T1[Thread 1] --> C1[Counter 0]
        T2[Thread 2] --> C2[Counter 1]
        T3[Thread 3] --> C3[Counter 2]
        T4[Thread 4] --> C4[Counter 3]
        
        subgraph CL1["Single Cache Line (64 bytes)"]
            C1
            C2
            C3
            C4
        end
    end
    
    subgraph Good["✓ Cache Line Aligned"]
        direction LR
        T1b[Thread 1] --> C1b[Counter 0]
        T2b[Thread 2] --> C2b[Counter 1]
        T3b[Thread 3] --> C3b[Counter 2]
        T4b[Thread 4] --> C4b[Counter 3]
        
        subgraph CL2["Cache Line 0"]
            C1b
            PAD1[padding...]
        end
        subgraph CL3["Cache Line 1"]
            C2b
            PAD2[padding...]
        end
        subgraph CL4["Cache Line 2"]
            C3b
            PAD3[padding...]
        end
        subgraph CL5["Cache Line 3"]
            C4b
            PAD4[padding...]
        end
    end
    
    style Bad fill:#ffcccc
    style Good fill:#ccffcc
```

## Contents

| File | Topic | Key Concept |
|------|-------|-------------|
| `src/aos_vs_soa.cpp` | Data Layout | Cache-friendly data organization |
| `src/false_sharing.cpp` | False Sharing | Multi-threaded cache contention |
| `src/alignment.cpp` | Memory Alignment | SIMD-friendly allocation |
| `src/prefetch.cpp` | Prefetching | Manual cache hints |

## Key Concepts

### AOS vs SOA

**Array of Structures (AOS):**
```cpp
struct Particle { float x, y, z, vx, vy, vz; };
std::vector<Particle> particles;
```

**Structure of Arrays (SOA):**
```cpp
struct Particles {
    std::vector<float> x, y, z, vx, vy, vz;
};
```

SOA is faster for sequential access because it maximizes cache line utilization.

### False Sharing

When threads modify data on the same cache line, they cause expensive cache invalidations:

```cpp
// Bad: counters share cache line
int counters[4];

// Good: each counter on its own cache line
alignas(64) int counters[4][16];  // Padded
```

### Memory Alignment

SIMD instructions require aligned memory:

```cpp
// Aligned allocation
alignas(32) float data[1024];

// Or use aligned_alloc
float* data = static_cast<float*>(std::aligned_alloc(32, 1024 * sizeof(float)));
```

### Prefetching

Hint the CPU to load data before it's needed:

```cpp
for (size_t i = 0; i < n; ++i) {
    __builtin_prefetch(&data[i + 64], 0, 3);  // Prefetch ahead
    process(data[i]);
}
```

## Running Benchmarks

```bash
# Build
cmake --preset=release
cmake --build build/release

# Run all memory benchmarks
./build/release/examples/02-memory-cache/bench/aos_soa_bench
./build/release/examples/02-memory-cache/bench/false_sharing_bench
./build/release/examples/02-memory-cache/bench/alignment_bench
./build/release/examples/02-memory-cache/bench/prefetch_bench
```

## Expected Results

| Benchmark | Expected Speedup |
|-----------|------------------|
| SOA vs AOS | 2-4x |
| Aligned vs Unaligned (false sharing) | 5-20x |
| Aligned vs Unaligned (SIMD) | 1.5-3x |
| With Prefetch vs Without | 1.1-1.5x |

Results vary by CPU architecture and data size.

## Profiling Tips

Check cache misses:
```bash
perf stat -e cache-misses,cache-references ./build/release/examples/02-memory-cache/bench/aos_soa_bench
```

## Further Reading

- [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)
- [Gallery of Processor Cache Effects](http://igoro.com/archive/gallery-of-processor-cache-effects/)

---

## Common Pitfalls

### ❌ Assuming AOS is always bad

AOS can be better when accessing all fields together (e.g., particle physics where you need x, y, z, vx, vy, vz simultaneously).

### ❌ Over-aligning everything

Only data accessed in hot loops needs alignment. Don't waste memory aligning rarely-used data.

### ❌ Prefetching too early

Prefetch distance must match your access pattern. Too early = data evicted before use. Too late = no benefit.

### ❌ Forgetting to measure

Always benchmark before and after. Some "optimizations" can hurt performance on certain CPUs.

---

## Knowledge Check

Test your understanding:

1. **When should you prefer SOA over AOS?**
   <details>
   <summary>Click for answer</summary>
   When you process one field at a time (e.g., updating all x-coordinates). SOA improves cache utilization when access patterns are field-wise.
   </details>

2. **What cache line size do most modern x86 CPUs use?**
   <details>
   <summary>Click for answer</summary>
   64 bytes. This is why `alignas(64)` is used to prevent false sharing.
   </details>

3. **Why does false sharing hurt multi-threaded performance?**
   <details>
   <summary>Click for answer</summary>
   When multiple threads write to variables on the same cache line, the cache coherence protocol causes the line to "bounce" between cores, serializing access that should be parallel.
   </details>

---

## Next Steps

- Continue to [Modern C++ Features](../03-modern-cpp/) to learn about compile-time optimization
- Read the [Optimization Playbook](../../docs/zh/guides/optimization-playbook.md) for systematic optimization
- Read the [Memory Layout Deep Dive](../../docs/zh/deep-dives/memory-layout.md) for in-depth analysis
