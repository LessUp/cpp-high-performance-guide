# API 参考

项目中可用的工具函数和辅助类。

---

## 概述

本文档记录以下位置的实用工具：
- `examples/02-memory-cache/include/memory_utils.hpp`
- `examples/04-simd-vectorization/include/simd_wrapper.hpp`
- `benchmarks/common/benchmark_utils.hpp`

---

## 内存工具

### 对齐分配

```cpp
namespace hpc::memory {

// 按指定对齐方式分配内存
void* aligned_alloc(size_t alignment, size_t size);

// 释放对齐内存
void aligned_free(void* ptr);

// 缓存行大小常量
constexpr size_t CACHE_LINE_SIZE = 64;

// 检查指针是否对齐
bool is_aligned(const void* ptr, size_t alignment);

}
```

### 缓存工具

```cpp
namespace hpc::memory {

// 预取数据到缓存
void prefetch(const void* ptr, int locality = 3);

// 缓存行大小（通常为 64 字节）
constexpr size_t CACHE_LINE_SIZE = 64;

// 填充结构体以防止伪共享
#define CACHE_LINE_PAD(name) char name##_pad[64 - sizeof(name)]

}
```

---

## 另请参阅

- [学习路径](../guides/learning-path.md)
- [性能分析指南](../guides/profiling-guide.md)
- [最佳实践](../guides/best-practices.md)
