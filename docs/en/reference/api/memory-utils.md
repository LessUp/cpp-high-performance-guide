# Memory Utilities API

Memory and cache optimization utilities for aligned allocation, cache-friendly data structures, and prefetching.

---

## Overview

**Header:** `examples/02-memory-cache/include/memory_utils.hpp`

**Namespace:** `hpc::memory`

---

## Constants

### CACHE_LINE_SIZE

```cpp
constexpr std::size_t CACHE_LINE_SIZE = 64;
```

Typical cache line size on x86/ARM architectures (64 bytes). Use for alignment and padding calculations.

**Example:**
```cpp
// Align a structure to cache line boundary
struct alignas(hpc::memory::CACHE_LINE_SIZE) AlignedCounter {
    std::atomic<int> value{0};
};
```

---

### PAGE_SIZE

```cpp
constexpr std::size_t PAGE_SIZE = 4096;
```

Default memory page size on most systems.

---

## Aligned Memory Allocation

### aligned_alloc

```cpp
void* aligned_alloc(std::size_t size, std::size_t alignment);
```

Allocate memory with specified alignment.

**Parameters:**
- `size` - Number of bytes to allocate
- `alignment` - Alignment requirement (must be power of 2)

**Returns:** Pointer to aligned memory, or `nullptr` on failure

**Example:**
```cpp
// Allocate 1024 floats aligned to 64-byte boundary
float* data = static_cast<float*>(
    hpc::memory::aligned_alloc(1024 * sizeof(float), 64)
);

// Use the memory...

hpc::memory::aligned_free(data);
```

---

### aligned_free

```cpp
void aligned_free(void* ptr);
```

Free memory allocated with `aligned_alloc`.

**Parameters:**
- `ptr` - Pointer to aligned memory

---

### aligned_unique_ptr

```cpp
template<typename T>
using aligned_unique_ptr = std::unique_ptr<T, AlignedDeleter>;
```

Smart pointer type for aligned memory with automatic cleanup.

---

### make_aligned

```cpp
template<typename T>
aligned_unique_ptr<T> make_aligned(
    std::size_t count,
    std::size_t alignment = CACHE_LINE_SIZE
);
```

Create aligned unique pointer with automatic memory management.

**Parameters:**
- `count` - Number of elements to allocate
- `alignment` - Alignment requirement (default: cache line size)

**Returns:** Unique pointer to aligned memory

**Example:**
```cpp
// Create aligned array of 1024 floats
auto data = hpc::memory::make_aligned<float>(1024, 64);

// Use data[0], data[1], etc.
for (size_t i = 0; i < 1024; ++i) {
    data[i] = static_cast<float>(i);
}
// Automatic cleanup when data goes out of scope
```

---

## Aligned STL Allocator

### AlignedAllocator

```cpp
template<typename T, std::size_t Alignment = CACHE_LINE_SIZE>
class AlignedAllocator;
```

STL-compatible allocator for aligned memory allocation.

**Template Parameters:**
- `T` - Element type
- `Alignment` - Memory alignment (default: 64 bytes)

**Example:**
```cpp
// Create aligned vector
std::vector<float, hpc::memory::AlignedAllocator<float, 64>> aligned_vec;

aligned_vec.resize(1024);
// Data is now 64-byte aligned, suitable for SIMD operations
```

---

### aligned_vector

```cpp
template<typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, CACHE_LINE_SIZE>>;
```

Convenience alias for aligned vector.

**Example:**
```cpp
hpc::memory::aligned_vector<float> data(1024);
// data is cache-line aligned for optimal SIMD performance
```

---

## Cache Line Padding

### CacheLinePadded

```cpp
template<typename T>
struct alignas(CACHE_LINE_SIZE) CacheLinePadded {
    T value;
    
    CacheLinePadded() = default;
    explicit CacheLinePadded(const T& v);
    explicit CacheLinePadded(T&& v);
    
    operator T&();
    operator const T&() const;
    T* operator->();
    const T* operator->() const;
};
```

Wrap any type to ensure it occupies a full cache line, preventing false sharing.

**Example:**
```cpp
// Create array of counters, each on its own cache line
std::array<hpc::memory::CacheLinePadded<std::atomic<int>>, 4> counters;

// Each counter is on a separate cache line
// No false sharing when different threads increment different counters
#pragma omp parallel for
for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 1000000; ++j) {
        counters[i].value.fetch_add(1, std::memory_order_relaxed);
    }
}
```

---

## Prefetch Functions

### prefetch_read

```cpp
template<typename T>
void prefetch_read(const T* ptr);
```

Prefetch data into cache for reading.

**Parameters:**
- `ptr` - Pointer to data to prefetch

**Example:**
```cpp
// Prefetch ahead in array traversal
for (size_t i = 0; i < n; ++i) {
    hpc::memory::prefetch_read(&data[i + 64]);  // Prefetch 64 elements ahead
    process(data[i]);
}
```

---

### prefetch_write

```cpp
template<typename T>
void prefetch_write(T* ptr);
```

Prefetch cache line for writing (exclusive ownership).

**Parameters:**
- `ptr` - Pointer to data to prefetch

---

### prefetch

```cpp
template<typename T>
void prefetch(const T* ptr, int locality = 3);
```

Prefetch with specified locality hint.

**Parameters:**
- `ptr` - Pointer to data to prefetch
- `locality` - Temporal locality hint:
  - `0` - Non-temporal (data used once, don't pollute cache)
  - `1` - Low temporal locality
  - `2` - Medium temporal locality
  - `3` - High temporal locality (default)

**Example:**
```cpp
// Non-temporal prefetch for sequential scan
for (size_t i = 0; i < n; ++i) {
    hpc::memory::prefetch(&data[i + 64], 0);  // Non-temporal
    sum += data[i];
}
```

---

## Quick Reference

| Function | Purpose | Use Case |
|----------|---------|----------|
| `aligned_alloc` | Raw aligned allocation | Manual memory management |
| `make_aligned` | Smart aligned allocation | RAII-style aligned memory |
| `AlignedAllocator` | STL-compatible allocator | Aligned containers |
| `CacheLinePadded` | Prevent false sharing | Multi-threaded counters |
| `prefetch_read` | Prefetch for reading | Array traversal |
| `prefetch_write` | Prefetch for writing | Preparing write buffers |

---

## See Also

- [Memory Module Examples](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/02-memory-cache)
- [Best Practices Guide](/en/guides/best-practices)
- [Optimization Decision Tree](/en/guides/optimization-decision-tree)
