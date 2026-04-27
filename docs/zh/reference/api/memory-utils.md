# 内存工具 API

用于对齐分配、缓存友好数据结构和预取的内存与缓存优化工具。

---

## 概述

**头文件：** `examples/02-memory-cache/include/memory_utils.hpp`

**命名空间：** `hpc::memory`

---

## 常量

### CACHE_LINE_SIZE

```cpp
constexpr std::size_t CACHE_LINE_SIZE = 64;
```

x86/ARM 架构上的典型缓存行大小（64 字节）。用于对齐和填充计算。

**示例：**
```cpp
// 将结构体对齐到缓存行边界
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

分配具有指定对齐的内存。

**参数：**
- `size` - 要分配的字节数
- `alignment` - 对齐要求（必须是 2 的幂）

**返回：** 指向对齐内存的指针，失败时返回 `nullptr`

**示例：**
```cpp
// 分配 1024 个浮点数，对齐到 64 字节边界
float* data = static_cast<float*>(
    hpc::memory::aligned_alloc(1024 * sizeof(float), 64)
);

// 使用内存...

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

带自动清理的对齐内存智能指针类型。

---

### make_aligned

```cpp
template<typename T>
aligned_unique_ptr<T> make_aligned(
    std::size_t count,
    std::size_t alignment = CACHE_LINE_SIZE
);
```

创建带自动内存管理的对齐唯一指针。

**参数：**
- `count` - 要分配的元素数量
- `alignment` - 对齐要求（默认：缓存行大小）

**返回：** 指向对齐内存的唯一指针

**示例：**
```cpp
// 创建 1024 个浮点数的对齐数组
auto data = hpc::memory::make_aligned<float>(1024, 64);

// 使用 data[0], data[1] 等
for (size_t i = 0; i < 1024; ++i) {
    data[i] = static_cast<float>(i);
}
// data 离开作用域时自动清理
```

---

## 对齐 STL 分配器

### AlignedAllocator

```cpp
template<typename T, std::size_t Alignment = CACHE_LINE_SIZE>
class AlignedAllocator;
```

用于对齐内存分配的 STL 兼容分配器。

**模板参数：**
- `T` - 元素类型
- `Alignment` - 内存对齐（默认：64 字节）

**示例：**
```cpp
// 创建对齐向量
std::vector<float, hpc::memory::AlignedAllocator<float, 64>> aligned_vec;

aligned_vec.resize(1024);
// 数据现在是 64 字节对齐，适合 SIMD 操作
```

---

### aligned_vector

```cpp
template<typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, CACHE_LINE_SIZE>>;
```

对齐向量的便捷别名。

**示例：**
```cpp
hpc::memory::aligned_vector<float> data(1024);
// data 是缓存行对齐的，适合 SIMD 性能优化
```

---

## 缓存行填充

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

包装任意类型以确保其占用完整缓存行，防止伪共享。

**示例：**
```cpp
// 创建计数器数组，每个在自己的缓存行上
std::array<hpc::memory::CacheLinePadded<std::atomic<int>>, 4> counters;

// 每个计数器在独立的缓存行上
// 不同线程递增不同计数器时不会发生伪共享
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

预取数据到缓存以供读取。

**参数：**
- `ptr` - 要预取的数据指针

**示例：**
```cpp
// 在数组遍历中提前预取
for (size_t i = 0; i < n; ++i) {
    hpc::memory::prefetch_read(&data[i + 64]);  // 提前 64 个元素预取
    process(data[i]);
}
```

---

### prefetch_write

```cpp
template<typename T>
void prefetch_write(T* ptr);
```

预取缓存行以供写入（独占所有权）。

**参数：**
- `ptr` - 要预取的数据指针

---

### prefetch

```cpp
template<typename T>
void prefetch(const T* ptr, int locality = 3);
```

带指定局部性提示的预取。

**参数：**
- `ptr` - 要预取的数据指针
- `locality` - 时间局部性提示：
  - `0` - 非时间性（数据使用一次，不污染缓存）
  - `1` - 低时间局部性
  - `2` - 中等时间局部性
  - `3` - 高时间局部性（默认）

**示例：**
```cpp
// 顺序扫描的非时间性预取
for (size_t i = 0; i < n; ++i) {
    hpc::memory::prefetch(&data[i + 64], 0);  // 非时间性
    sum += data[i];
}
```

---

## 快速参考

| 函数 | 用途 | 使用场景 |
|------|------|----------|
| `aligned_alloc` | 原始对齐分配 | 手动内存管理 |
| `make_aligned` | 智能对齐分配 | RAII 风格对齐内存 |
| `AlignedAllocator` | STL 兼容分配器 | 对齐容器 |
| `CacheLinePadded` | 防止伪共享 | 多线程计数器 |
| `prefetch_read` | 预取以供读取 | 数组遍历 |
| `prefetch_write` | 预取以供写入 | 准备写入缓冲区 |

---

## 另见

- [内存模块示例](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/02-memory-cache)
- [最佳实践指南](/zh/guides/best-practices)
- [优化决策树](/zh/guides/optimization-decision-tree)
