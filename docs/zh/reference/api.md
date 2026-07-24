# API 参考

本页汇总仓库中所有公共头文件的 API。每个条目包含签名、简要说明和用法示例。

---

## hpc::core (`include/hpc/core.hpp`)

平台检测工具，提供缓存行、页面大小和硬件并发度等基础常量。

### `CACHE_LINE_SIZE`

```cpp
constexpr std::size_t CACHE_LINE_SIZE = 64;
```

编译期缓存行大小常量（字节）。适用于 `alignas` 等需要编译期常量的场景。大多数 x86/ARM 系统为 64 字节，Apple M 系列为 128 字节。

```cpp
struct alignas(hpc::core::CACHE_LINE_SIZE) PaddedCounter {
    int64_t value;
};
```

### `cache_line_size()`

```cpp
inline std::size_t cache_line_size();
```

运行时获取缓存行大小。优先使用 `std::hardware_destructive_interference_size`（C++17），否则回退到 64。

```cpp
std::size_t cls = hpc::core::cache_line_size();
```

### `PAGE_SIZE`

```cpp
constexpr std::size_t PAGE_SIZE = 4096;
```

编译期页面大小常量（字节），适用于大多数系统。

### `page_size()`

```cpp
inline std::size_t page_size();
```

运行时获取系统页面大小。在 POSIX 系统上调用 `sysconf(_SC_PAGESIZE)`，Windows 上调用 `GetSystemInfo`，回退值为 4096。

```cpp
std::size_t ps = hpc::core::page_size();
```

### `hardware_concurrency()`

```cpp
inline unsigned int hardware_concurrency();
```

返回硬件支持的并发线程数。封装 `std::thread::hardware_concurrency()`，保证返回值至少为 1。

```cpp
unsigned int n = hpc::core::hardware_concurrency();
```

---

## hpc::simd (`include/hpc/simd.hpp`)

SIMD 向量化封装，提供跨指令集的可移植向量类型和高级数组操作。

### `SIMDLevel` 枚举

```cpp
enum class SIMDLevel { Scalar, SSE2, AVX, AVX2, AVX512 };
```

表示当前 CPU 支持的最高 SIMD 指令集级别。

### `detect_simd_level()`

```cpp
inline SIMDLevel detect_simd_level();
```

运行时检测 CPU 支持的最高 SIMD 级别。在 GCC/Clang x86 上使用 `__builtin_cpu_supports`，其他平台根据编译宏判断。

```cpp
auto level = hpc::simd::detect_simd_level();
std::cout << hpc::simd::simd_level_name(level) << "\n";  // e.g. "AVX2"
```

### `simd_level_name()`

```cpp
inline const char* simd_level_name(SIMDLevel level);
```

将 `SIMDLevel` 转为可读字符串（`"Scalar"`、`"SSE2"`、`"AVX"`、`"AVX2"`、`"AVX-512"`）。

### `SimdVec<T, Width>`

```cpp
template <typename T, std::size_t Width>
class SimdVec;
```

固定宽度的 SIMD 向量类型。在 x86 上针对 `float` 有 SSE2（Width=4）、AVX2（Width=8）、AVX-512（Width=16）的特化实现，使用对应的 intrinsics；其他情况回退到标量实现 `SimdVecScalar<T, Width>`。

**主要成员：**

| 成员 | 签名 | 说明 |
|------|------|------|
| 默认构造 | `SimdVec()` | 初始化为零 |
| 广播构造 | `explicit SimdVec(T value)` | 所有通道填充同一值 |
| 指针构造 | `explicit SimdVec(const T* ptr)` | 从内存非对齐加载 |
| 对齐加载 | `static SimdVec load_aligned(const T* ptr)` | 从对齐内存加载 |
| 存储 | `void store(T* ptr) const` | 非对齐写入 |
| 对齐存储 | `void store_aligned(T* ptr) const` | 对齐写入 |
| 下标访问 | `T operator[](std::size_t i) const` | 读取第 i 个通道 |
| 算术运算 | `operator+`, `-`, `*`, `/`, `+=`, `-=`, `*=` | 逐通道运算 |
| 水平求和 | `T horizontal_sum() const` | 所有通道求和 |
| 融合乘加 | `static SimdVec fmadd(a, b, c)` | 计算 `a * b + c` |
| 平方根 | `SimdVec sqrt() const` | 逐通道 `sqrt` |
| 逐通道最小 | `SimdVec min(const SimdVec& other) const` | 逐通道取最小值 |
| 逐通道最大 | `SimdVec max(const SimdVec& other) const` | 逐通道取最大值 |

```cpp
using Vec8 = hpc::simd::SimdVec<float, 8>;  // AVX2 特化

float a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
float b[8] = {8, 7, 6, 5, 4, 3, 2, 1};

Vec8 va(a), vb(b);
Vec8 vc = va + vb;          // 逐通道加法
float sum = vc.horizontal_sum();  // 所有通道求和

Vec8 vf = Vec8::fmadd(va, vb, vc);  // va * vb + vc
```

### `FloatVec` / `FLOAT_VEC_WIDTH`

```cpp
using FloatVec = SimdVec<float, /* 平台相关 */>;
constexpr std::size_t FLOAT_VEC_WIDTH = /* 4, 8 或 16 */;
```

当前平台最优的 `float` 向量类型别名。AVX-512 下为 `SimdVec<float, 16>`，AVX2 下为 `SimdVec<float, 8>`，SSE2 下为 `SimdVec<float, 4>`，无 SIMD 时回退到 `SimdVecScalar<float, 4>`。

```cpp
hpc::simd::FloatVec v(1.0f);  // 广播 1.0 到所有通道
// FLOAT_VEC_WIDTH 告诉你一次处理多少个 float
```

### `AlignedAllocator<T>` / `aligned_vector<T>`

```cpp
template <typename T>
class AlignedAllocator;  // 按 SIMD 宽度对齐的分配器

template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T>>;
```

按当前 SIMD 级别对齐的 STL 分配器和向量别名。对齐值由 `get_simd_alignment()` 决定（AVX-512: 64, AVX2/AVX: 32, SSE2: 16）。

```cpp
hpc::simd::aligned_vector<float> data(1024);  // SIMD 对齐的 float 数组
```

### `make_aligned_vector()`

```cpp
template <typename T>
aligned_vector<T> make_aligned_vector(std::size_t size);

template <typename T>
aligned_vector<T> make_aligned_vector(std::size_t size, const T& value);
```

创建 SIMD 对齐向量的便捷工厂函数。

```cpp
auto v = hpc::simd::make_aligned_vector<float>(1024, 0.0f);
```

### 高级数组操作

以下函数使用 `FloatVec` 封装，自动处理主循环 + 尾部标量回退。

#### `add_arrays_wrapped()`

```cpp
inline void add_arrays_wrapped(const float* a, const float* b, float* c, std::size_t n);
```

逐元素加法 `c[i] = a[i] + b[i]`，使用 `FloatVec` 向量化。

```cpp
hpc::simd::add_arrays_wrapped(a.data(), b.data(), c.data(), n);
```

#### `dot_product_wrapped()`

```cpp
inline float dot_product_wrapped(const float* a, const float* b, std::size_t n);
```

向量化点积，使用 `fmadd` 累加，最后 `horizontal_sum` 归约。

```cpp
float result = hpc::simd::dot_product_wrapped(a.data(), b.data(), n);
```

#### `scale_array_wrapped()`

```cpp
inline void scale_array_wrapped(float* arr, float scalar, std::size_t n);
```

原地标量乘法 `arr[i] *= scalar`。

```cpp
hpc::simd::scale_array_wrapped(arr.data(), 2.5f, n);
```

#### `clamp_array_wrapped()`

```cpp
inline void clamp_array_wrapped(float* arr, float min_val, float max_val, std::size_t n);
```

原地截断 `arr[i] = clamp(arr[i], min_val, max_val)`，使用 `max` + `min` 向量化。

```cpp
hpc::simd::clamp_array_wrapped(arr.data(), 0.0f, 1.0f, n);
```

---

## hpc::memory (`examples/02-memory-cache/include/memory_utils.hpp`)

内存对齐与缓存友好数据结构工具。

### `aligned_alloc()` / `aligned_free()`

```cpp
inline void* aligned_alloc(std::size_t size, std::size_t alignment);
inline void aligned_free(void* ptr);
```

分配/释放指定对齐的原始内存。POSIX 上使用 `posix_memalign`，Windows 上使用 `_aligned_malloc`。`alignment` 必须为 2 的幂。

```cpp
void* ptr = hpc::memory::aligned_alloc(1024 * sizeof(float), 64);
// ... 使用 ptr ...
hpc::memory::aligned_free(ptr);
```

### `make_aligned<T>()`

```cpp
template <typename T>
aligned_unique_ptr<T> make_aligned(std::size_t count,
                                   std::size_t alignment = hpc::core::CACHE_LINE_SIZE);
```

创建对齐的 `unique_ptr<T[]>`，自动构造和析构元素。默认按缓存行对齐。返回类型为 `std::unique_ptr<T[], AlignedDeleter<T>>`。

```cpp
auto buf = hpc::memory::make_aligned<float>(1024);       // 缓存行对齐
auto buf2 = hpc::memory::make_aligned<double>(512, 64);  // 64 字节对齐
```

### `AlignedAllocator<T, Alignment>` / `aligned_vector<T>`

```cpp
template <typename T, std::size_t Alignment = hpc::core::CACHE_LINE_SIZE>
class AlignedAllocator;

template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, hpc::core::CACHE_LINE_SIZE>>;
```

按缓存行对齐的 STL 分配器，默认对齐为 `CACHE_LINE_SIZE`（64 字节）。主要用于消除多线程伪共享。

> 注意：与 `hpc::simd::AlignedAllocator` 不同，此分配器按缓存行对齐而非 SIMD 宽度对齐。

```cpp
hpc::memory::aligned_vector<int64_t> counters(num_threads);
```

### `CacheLinePadded<T>`

```cpp
template <typename T>
struct alignas(hpc::core::CACHE_LINE_SIZE) CacheLinePadded {
    T value;
    // 支持隐式转换到 T&、operator->
};
```

将类型 `T` 填充到缓存行大小，防止伪共享。支持隐式转换和 `operator->`。

```cpp
struct alignas(64) ThreadCounter {
    hpc::memory::CacheLinePadded<int64_t> count{0};
};
// 每个 ThreadCounter 实例至少占 64 字节，避免相邻线程共享缓存行
```

### 软件预取

`memory_utils.hpp` 不提供独立的预取封装函数。示例中直接使用 GCC/Clang 内建函数：

```cpp
// 读预取，高时间局部性
__builtin_prefetch(ptr, 0, 3);

// 写预取
__builtin_prefetch(ptr, 1, 3);
```

参见 `examples/02-memory-cache/src/prefetch.cpp` 中的完整用法。

---

## hpc::bench (`benchmarks/common/benchmark_utils.hpp`)

基准测试辅助工具。

> **注意：** `DoNotOptimize` 和 `ClobberMemory` 由 Google Benchmark 提供（`benchmark::DoNotOptimize`、`benchmark::ClobberMemory`），不在 `hpc::bench` 命名空间中。

### `benchmark::DoNotOptimize`

```cpp
// 来自 <benchmark/benchmark.h>
template <typename Tp>
void benchmark::DoNotOptimize(Tp&& value);
```

阻止编译器优化掉 `value` 的计算。在基准测试循环中用于确保被测代码不被消除。

```cpp
for (auto _ : state) {
    auto result = expensive_computation();
    benchmark::DoNotOptimize(result);
}
```

### `benchmark::ClobberMemory`

```cpp
// 来自 <benchmark/benchmark.h>
void benchmark::ClobberMemory();
```

告知编译器所有内存均已被修改，阻止跨调用的内存优化。通常与 `DoNotOptimize` 配合使用。

```cpp
for (auto _ : state) {
    write_to_buffer(buf.data());
    benchmark::DoNotOptimize(buf.data());
    benchmark::ClobberMemory();
}
```

### `Timer`

```cpp
class hpc::bench::Timer {
public:
    void start();
    void stop();
    double elapsed_ns() const;
    double elapsed_us() const;
    double elapsed_ms() const;
    double elapsed_s() const;
};
```

基于 `std::chrono::high_resolution_clock` 的手动计时器。适用于 Google Benchmark 之外的简单计时场景。

```cpp
hpc::bench::Timer timer;
timer.start();
// ... 被测代码 ...
timer.stop();
std::printf("elapsed: %.2f ms\n", timer.elapsed_ms());
```

### `calculate_speedup()`

```cpp
inline double calculate_speedup(double baseline_time, double optimized_time);
```

计算加速比 `baseline_time / optimized_time`。`optimized_time <= 0` 时返回 0。

```cpp
double speedup = hpc::bench::calculate_speedup(baseline_ns, optimized_ns);
```

### `format_throughput()`

```cpp
inline std::string format_throughput(double bytes_per_second);
```

将字节/秒格式化为可读字符串（自动选择 B/s、KB/s、MB/s、GB/s、TB/s）。

```cpp
std::string tp = hpc::bench::format_throughput(1.5e9);  // "1.40 GB/s"
```

### `format_time()`

```cpp
inline std::string format_time(double nanoseconds);
```

将纳秒格式化为可读字符串（自动选择 ns、us、ms、s）。

```cpp
std::string t = hpc::bench::format_time(1234567.0);  // "1.23 ms"
```
