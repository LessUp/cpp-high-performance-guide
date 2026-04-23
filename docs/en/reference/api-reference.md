# API Reference

Utility functions and helper classes available in the project.

---

## Overview

This reference documents utilities found in:
- `examples/02-memory-cache/include/memory_utils.hpp`
- `examples/04-simd-vectorization/include/simd_wrapper.hpp`
- `benchmarks/common/benchmark_utils.hpp`

---

## API Modules

### [Memory Utilities](api/memory-utils.md)

Aligned allocation, cache-friendly containers, and prefetching.

**Key Functions:**
- `aligned_alloc()` - Allocate aligned memory
- `aligned_free()` - Free aligned memory
- `make_aligned()` - Smart aligned allocation
- `CacheLinePadded` - Prevent false sharing
- `prefetch_read()` / `prefetch_write()` - Cache hints

**Namespace:** `hpc::memory`

---

### [SIMD Wrapper](api/simd-wrapper.md)

Portable SIMD abstraction with operator overloading.

**Key Classes:**
- `SimdVec<T, Width>` - SIMD vector class
- `FloatVec` - Default float vector type

**Key Operations:**
- Arithmetic: `+`, `-`, `*`, `/`
- `horizontal_sum()` - Reduce to scalar
- `fmadd()` - Fused multiply-add
- `sqrt()`, `min()`, `max()`

**Namespace:** `hpc::simd`

---

### [Benchmark Utilities](api/benchmark-utils.md)

Benchmarking helpers and result export.

**Key Functions:**
- `DoNotOptimize()` - Prevent optimization
- `ClobberMemory()` - Memory barrier
- `export_to_json()` - Save results
- `calculate_speedup()` - Compare times

**Key Classes:**
- `BenchmarkResult` - Single result
- `BenchmarkSuite` - Result collection
- `Timer` - Manual timing

**Namespace:** `hpc::bench`

---

## Quick Reference

| Header | Namespace | Purpose |
|--------|-----------|---------|
| `memory_utils.hpp` | `hpc::memory` | Memory optimization |
| `simd_wrapper.hpp` | `hpc::simd` | SIMD operations |
| `benchmark_utils.hpp` | `hpc::bench` | Benchmarking |

---

## See Also

- [Learning Path](../guides/learning-path.md)
- [Profiling Guide](../guides/profiling-guide.md)
- [Best Practices](../guides/best-practices.md)
