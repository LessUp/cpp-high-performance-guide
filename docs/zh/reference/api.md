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

## hpc::memory (`include/hpc/memory_utils.hpp`)

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

```cpp
template <typename T>
inline void prefetch_read(const T* ptr);  // 读预取，高时间局部性；不支持的编译器上为空操作

inline int64_t sum_no_prefetch(const int64_t* data, std::size_t n);
inline int64_t sum_with_prefetch(const int64_t* data, std::size_t n);
inline int64_t sum_random_no_prefetch(const int64_t* data, const std::size_t* indices, std::size_t n);
inline int64_t sum_random_with_prefetch(const int64_t* data, const std::size_t* indices, std::size_t n);
```

`prefetch_read()` 是 `__builtin_prefetch(ptr, 0, 3)` 的可移植封装。四个求和函数是
预取对照实验的共享实现（`examples/02-memory-cache/` 的示例与基准消费同一份代码）：
顺序访问时硬件预取器通常已足够高效，软件预取收益很小；随机（gather）访问时
未来的目标地址可从索引数组提前得知，软件预取能掩盖内存延迟——这正是
`sum_random_with_prefetch` 演示的场景。

完整演示参见 `examples/02-memory-cache/src/prefetch.cpp`。

### Particle 数据布局（`include/hpc/particle_types.hpp`）

用于对比 Array of Structures（AOS）与 Structure of Arrays（SOA）内存布局的教学数据结构，位于 `hpc::memory` 命名空间。

> **注意：** 本模块为教学示例，非生产就绪代码。

#### `ParticleAOS`

```cpp
struct ParticleAOS {
    float x, y, z;     // 位置
    float vx, vy, vz;  // 速度
};
```

每个粒子连续存储自身全部字段。更新位置时也会把速度加载进缓存，浪费带宽。

#### `ParticleSOA`

```cpp
struct ParticleSOA {
    std::vector<float> x, y, z;     // 位置
    std::vector<float> vx, vy, vz;  // 速度
    void resize(size_t n);
    size_t size() const;
};
```

每个字段独立连续存储，更新位置只触碰位置数组，缓存利用率高且易于向量化。

#### 更新与能量计算

```cpp
inline void update_particles_aos(std::vector<ParticleAOS>& particles, float dt);
inline float compute_energy_aos(const std::vector<ParticleAOS>& particles);

inline void update_particles_soa(ParticleSOA& particles, float dt);
inline float compute_energy_soa(const ParticleSOA& particles);
```

按 `位置 += 速度 * dt` 更新，或计算总动能 `0.5 * Σ(v²)`。

#### `initialize_particles()`

```cpp
inline void initialize_particles(std::vector<ParticleAOS>& particles, size_t n);
inline void initialize_particles(ParticleSOA& particles, size_t n);
```

用确定性值（非随机，避免 `<random>` 依赖）初始化 `n` 个粒子。

```cpp
hpc::memory::ParticleSOA particles;
hpc::memory::initialize_particles(particles, 10000);
hpc::memory::update_particles_soa(particles, 0.016f);
```

---

## hpc::instrumentation (`include/hpc/instrumentation.hpp`)

可注入的操作计数器，用 RAII 作用域管理，取代散落在 `Buffer`/`CountingAllocator` 中的全局静态计数器。测试或示例创建局部 `OperationMetrics`，按指针传入被观察模块；指针为 `nullptr` 时零开销。

### `OperationMetrics`

```cpp
class OperationMetrics {
public:
    size_t copy_count = 0;
    size_t move_count = 0;
    size_t allocation_count = 0;
    size_t deallocation_count = 0;
    size_t total_bytes_allocated = 0;
    size_t total_bytes_deallocated = 0;

    void reset() noexcept;
    void record_copy() noexcept;
    void record_move() noexcept;
    void record_allocation(size_t bytes) noexcept;
    void record_deallocation(size_t bytes) noexcept;
};
```

### `OperationMetrics::Scope`

```cpp
class OperationMetrics::Scope {
public:
    explicit Scope(OperationMetrics& m);  // 构造时调用 reset()
    OperationMetrics& metrics() noexcept;
    // 不可拷贝、不可移动
};
```

RAII 作用域，构造时重置计数器，保证每个测试块从干净状态开始，无需手动 `reset()`。

```cpp
hpc::instrumentation::OperationMetrics metrics;
hpc::instrumentation::OperationMetrics::Scope scope(metrics);
// 被测代码在此运行，结束后读取 metrics.copy_count 等
```

---

## hpc::move_semantics (`include/hpc/buffer.hpp`)

演示拷贝（昂贵，O(n) 内存拷贝）与移动（廉价，O(1) 指针转移）差异的缓冲区类。

### `Buffer`

```cpp
class Buffer {
public:
    Buffer();
    explicit Buffer(size_t size, instrumentation::OperationMetrics* metrics = nullptr);
    Buffer(const Buffer& other, instrumentation::OperationMetrics* metrics = nullptr);
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(const Buffer& other);
    Buffer& operator=(Buffer&& other) noexcept;

    size_t size() const;
    bool empty() const;
    char* data();
    const char* data() const;
};
```

拥有动态分配缓冲区，RAII 释放。拷贝构造/赋值触发 `memcpy` 并通过可选 `metrics` 记录拷贝事件；移动仅转移指针并记录移动事件。

### 拷贝 vs 引用传递示例函数

```cpp
inline void process_by_copy(Buffer buf);        // 按值传递，触发拷贝
inline void process_by_ref(const Buffer& buf);  // 按引用传递，无拷贝
inline void observe_buffer(const Buffer& buf);  // 读取首字节，避免被优化
```

```cpp
hpc::move_semantics::Buffer src(1024, &metrics);
hpc::move_semantics::process_by_copy(src);           // metrics.copy_count 增加
hpc::move_semantics::Buffer moved = std::move(src);  // metrics.move_count 增加
```

---

## hpc::vector_reserve (`include/hpc/vector_reserve.hpp`)

通过自定义分配器追踪 `std::vector` 的容量增长与分配次数。

### `CountingAllocator<T>`

```cpp
template <typename T>
class CountingAllocator {
public:
    using value_type = T;
    CountingAllocator() noexcept;
    explicit CountingAllocator(instrumentation::OperationMetrics* metrics) noexcept;
    template <typename U> CountingAllocator(const CountingAllocator<U>& other) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* ptr, std::size_t n) noexcept;
    instrumentation::OperationMetrics* metrics() const noexcept;
};
```

基于 `std::malloc/free` 的分配器，分配/释放时通过注入的 `metrics` 记录次数与字节数。`metrics` 为 `nullptr` 时退化为普通分配，零观察开销。

### `CountingVector<T>`

```cpp
template <typename T>
using CountingVector = std::vector<T, CountingAllocator<T>>;
```

```cpp
hpc::instrumentation::OperationMetrics metrics;
hpc::vector_reserve::CountingAllocator<int> alloc(&metrics);
hpc::vector_reserve::CountingVector<int> v(alloc);
v.reserve(1000);  // metrics.allocation_count 增加
```

---

## hpc::compile_time (`include/hpc/compile_time.hpp`)

C++20 `constexpr`/`consteval` 编译期计算工具：阶乘、查找表、哈希、素数。

### 阶乘

```cpp
inline int64_t factorial_runtime(int n);      // 运行期
constexpr int64_t factorial_constexpr(int n); // 可能编译期
consteval int64_t factorial_consteval(int n); // 必须编译期（C++20）
```

```cpp
constexpr auto f = hpc::compile_time::factorial_constexpr(10);  // 编译期求值
```

### 正弦查找表

```cpp
template <size_t N>
constexpr std::array<double, N> generate_sin_table();
constexpr auto SIN_TABLE = generate_sin_table<1024>();
inline double fast_sin(double angle);
```

编译期用泰勒展开生成 1024 项正弦表，`fast_sin` 运行期查表（非 `constexpr`，因含取整归一化）。

```cpp
double y = hpc::compile_time::fast_sin(1.23);
```

### FNV-1a 哈希

```cpp
constexpr uint64_t fnv1a_hash(const char* str);
consteval uint64_t operator""_hash(const char* str, size_t);
```

```cpp
constexpr uint64_t h = "hello"_hash;  // 编译期哈希
```

### 素数

```cpp
constexpr bool is_prime(int n);
template <size_t N> constexpr std::array<int, N> generate_primes();
constexpr auto FIRST_100_PRIMES = generate_primes<100>();
```

---

## hpc::ranges (`include/hpc/ranges_utils.hpp`)

C++20 Ranges 对比教学模块，每个操作提供 raw loop / 经典算法 / ranges 三种实现并排对比。

> **注意：** 教学示例，接口刻意暴露实现策略，价值在于并排对比而非抽象深度。

### Transform / Filter / Sum

```cpp
inline void transform_raw_loop(const std::vector<int>& in, std::vector<int>& out);
inline void transform_algorithm(const std::vector<int>& in, std::vector<int>& out);
inline void transform_ranges(const std::vector<int>& in, std::vector<int>& out);

inline std::vector<int> filter_raw_loop(const std::vector<int>& input);
inline std::vector<int> filter_algorithm(const std::vector<int>& input);
inline auto filter_ranges_view(const std::vector<int>& input);  // 惰性视图

inline int64_t sum_raw_loop(const std::vector<int>& input);
inline int64_t sum_algorithm(const std::vector<int>& input);
inline int64_t sum_ranges(const std::vector<int>& input);
```

### 链式操作与物化

```cpp
inline std::vector<int> chain_raw_loop(const std::vector<int>& input);
inline auto chain_ranges_view(const std::vector<int>& input);  // 惰性、单趟

template <std::ranges::range R>
std::vector<std::ranges::range_value_t<R>> to_vector(R&& range);
```

`to_vector` 将惰性视图物化为 `std::vector`。

```cpp
auto view = hpc::ranges::chain_ranges_view(data);
auto result = hpc::ranges::to_vector(view);
```

---

## hpc::concurrency (`include/hpc/concurrency_utils.hpp`, `include/hpc/lock_free_queue.hpp`)

并发与多线程工具：缓存行对齐的原子计数器、自旋锁、并行执行助手，以及无锁 SPSC/MPMC 队列。

### `AlignedCounter`

```cpp
struct alignas(hpc::core::CACHE_LINE_SIZE) AlignedCounter {
    std::atomic<int64_t> value{0};
    void increment(std::memory_order order = std::memory_order_seq_cst);
    int64_t load(std::memory_order order = std::memory_order_seq_cst) const;
    void store(int64_t v, std::memory_order order = std::memory_order_seq_cst);
};
```

缓存行对齐的原子计数器，避免伪共享。等价于 `CacheLinePadded<std::atomic<int64_t>>`，提供常用原子操作的便捷封装。

### `SpinLock`

```cpp
class SpinLock {
public:
    SpinLock() noexcept;
    void lock();
    void unlock();
    bool try_lock();
    // 不可拷贝、不可移动
};
```

基于 `std::atomic_flag` 的自旋锁，满足 BasicLockable 与 Lockable 要求，可用于 `std::lock_guard`/`std::unique_lock`/`std::scoped_lock`。适用于极短临界区。

```cpp
hpc::concurrency::SpinLock lock;
{
    std::lock_guard guard(lock);
    // 临界区
}
```

### `run_parallel()`

```cpp
template <typename Func>
double run_parallel(Func&& func, unsigned int num_threads);
```

在 `num_threads` 个线程上并发执行 `func`（每个线程以线程索引为参数调用一次），返回总耗时（毫秒）。

### `SPSCQueue<T, Capacity>`

```cpp
template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity >= 2, "Capacity must be at least 2");
public:
    bool push(const T& value);
    bool push(T&& value);
    std::optional<T> pop();
    bool empty() const;                   // 近似，可能陈旧
    size_t size() const;                  // 近似
    constexpr size_t capacity() const;    // == Capacity - 1
};
```

无锁有界队列，仅支持单生产者单消费者。环形缓冲区 + 原子 head/tail，acquire-release 序。`Capacity` 必须为 2 的幂且 ≥ 2；恒留一空槽，实际容量为 `Capacity - 1`。

### `MPMCQueue<T, Capacity>`

```cpp
template <typename T, size_t Capacity>
class MPMCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
public:
    bool push(const T& value);
    bool push(T&& value);
    std::optional<T> pop();
};
```

支持多生产者多消费者的无锁有界队列，用序列号协调 cell。`Capacity` 必须为 2 的幂。

```cpp
hpc::concurrency::SPSCQueue<int, 1024> q;
q.push(42);
auto v = q.pop();  // std::optional<int>，含 42
```

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

基于 `std::chrono::steady_clock` 的手动计时器。`steady_clock` 保证单调，不受系统时间调整（NTP）影响；`high_resolution_clock` 在部分标准库实现上等价于可回拨的 `system_clock`，不适合测量时间间隔。适用于 Google Benchmark 之外的简单计时场景。

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
