# Memory 工具 API

用于对齐分配、cache 友好数据结构以及预取的内存与 cache 优化工具。

---

## 概览

**头文件：** `examples/02-memory-cache/include/memory_utils.hpp`

**命名空间：** `hpc::memory`

---

## 常量

### CACHE_LINE_SIZE

```cpp
constexpr std::size_t CACHE_LINE_SIZE = 64;
```

x86/ARM 架构上典型的 cache line 大小（64 字节）。用于对齐与填充计算。

**示例：**
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

大多数系统上的默认内存页大小。

---

## 对齐内存分配

### aligned_alloc

```cpp
void* aligned_alloc(std::size_t size, std::size_t alignment);
```

以指定对齐方式分配内存。

**参数：**
- `size` - 要分配的字节数
- `alignment` - 对齐要求（必须为 2 的幂）

**返回值：** 指向对齐内存的指针，失败时返回 `nullptr`

**示例：**
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

释放由 `aligned_alloc` 分配的内存。

**参数：**
- `ptr` - 指向对齐内存的指针

---

### aligned_unique_ptr

```cpp
template<typename T>
using aligned_unique_ptr = std::unique_ptr<T, AlignedDeleter>;
```

用于对齐内存的智能指针类型，具备自动清理功能。

---

### make_aligned

```cpp
template<typename T>
aligned_unique_ptr<T> make_aligned(
    std::size_t count,
    std::size_t alignment = CACHE_LINE_SIZE
);
```

创建带自动内存管理的对齐 unique pointer。

**参数：**
- `count` - 要分配的元素数量
- `alignment` - 对齐要求（默认：cache line 大小）

**返回值：** 指向对齐内存的 unique pointer

**示例：**
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

## 对齐 STL 分配器

### AlignedAllocator

```cpp
template<typename T, std::size_t Alignment = CACHE_LINE_SIZE>
class AlignedAllocator;
```

STL 兼容的对齐内存分配器。

**模板参数：**
- `T` - 元素类型
- `Alignment` - 内存对齐（默认：64 字节）

**示例：**
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

对齐 vector 的便捷别名。

**示例：**
```cpp
hpc::memory::aligned_vector<float> data(1024);
// data is cache-line aligned for optimal SIMD performance
```

---

## Cache Line 填充

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

包装任意类型以确保其占据完整的 cache line，从而防止 false sharing。

**示例：**
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

## 预取函数

### prefetch_read

```cpp
template<typename T>
void prefetch_read(const T* ptr);
```

将数据预取到 cache 中以供读取。

**参数：**
- `ptr` - 指向要预取数据的指针

**示例：**
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

为写入预取 cache line（独占所有权）。

**参数：**
- `ptr` - 指向要预取数据的指针

---

### prefetch

```cpp
template<typename T>
void prefetch(const T* ptr, int locality = 3);
```

以指定的局部性提示进行预取。

**参数：**
- `ptr` - 指向要预取数据的指针
- `locality` - 时间局部性提示：
  - `0` - 非时间性（数据仅使用一次，不污染 cache）
  - `1` - 低时间局部性
  - `2` - 中等时间局部性
  - `3` - 高时间局部性（默认）

**示例：**
```cpp
// Non-temporal prefetch for sequential scan
for (size_t i = 0; i < n; ++i) {
    hpc::memory::prefetch(&data[i + 64], 0);  // Non-temporal
    sum += data[i];
}
```

---

## 快速参考

| 函数 | 用途 | 使用场景 |
|----------|---------|----------|
| `aligned_alloc` | 原始对齐分配 | 手动内存管理 |
| `make_aligned` | 智能对齐分配 | RAII 风格的对齐内存 |
| `AlignedAllocator` | STL 兼容分配器 | 对齐容器 |
| `CacheLinePadded` | 防止 false sharing | 多线程计数器 |
| `prefetch_read` | 为读取预取 | 数组遍历 |
| `prefetch_write` | 为写入预取 | 准备写入缓冲区 |

---

## 另请参阅

- [Memory 模块示例](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/02-memory-cache)
- [最佳实践指南](/zh/guides/best-practices)
- [优化决策树](/zh/guides/optimization-decision-tree)
