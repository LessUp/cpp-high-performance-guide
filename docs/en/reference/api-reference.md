# API Reference

Utility functions and helper classes available in the project.

---

## Overview

This reference documents utilities found in:
- `examples/02-memory-cache/include/memory_utils.hpp`
- `examples/04-simd-vectorization/include/simd_wrapper.hpp`
- `benchmarks/common/benchmark_utils.hpp`

---

## Memory Utilities

### Aligned Allocation

```cpp
namespace hpc::memory {

// Allocate memory with specified alignment
void* aligned_alloc(size_t alignment, size_t size);

// Free aligned memory
void aligned_free(void* ptr);

// Cache line size constant
constexpr size_t CACHE_LINE_SIZE = 64;

// Check if pointer is aligned
bool is_aligned(const void* ptr, size_t alignment);

}
```

### Cache Utilities

```cpp
namespace hpc::memory {

// Prefetch data into cache
void prefetch(const void* ptr, int locality = 3);

// Cache line size (typically 64 bytes)
constexpr size_t CACHE_LINE_SIZE = 64;

// Pad a struct to prevent false sharing
#define CACHE_LINE_PAD(name) char name##_pad[64 - sizeof(name)]

}
```

---

## See Also

- [Learning Path](../guides/learning-path.md)
- [Profiling Guide](../guides/profiling-guide.md)
- [Best Practices](../guides/best-practices.md)
