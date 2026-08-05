# C++20 性能实践

## 编译期计算

将计算从运行时移到编译期是最彻底的优化：运行时开销为零。C++20 的 `constexpr` 和 `consteval` 提供了两种保证级别。

### constexpr vs consteval

`include/hpc/compile_time.hpp`：

```cpp
// 可在编译期求值（如果参数是编译期常量）
constexpr int64_t factorial_constexpr(int n) {
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// 必须在编译期求值（C++20），否则编译错误
consteval int64_t factorial_consteval(int n) {
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}
```

`consteval` 的价值在于**强制保证**：如果某个函数被意外传入运行时变量，编译器立即报错而非静默退化为运行时计算。

### 编译期查找表

`generate_sin_table<1024>()` 在编译期用 Taylor 级数生成 1024 个正弦值：

```cpp
template <size_t N>
constexpr std::array<double, N> generate_sin_table() {
    std::array<double, N> table{};
    for (size_t i = 0; i < N; ++i) {
        double angle = (2.0 * PI * static_cast<double>(i)) / static_cast<double>(N);
        // Taylor series: sin(x) = x - x^3/3! + x^5/5! - ...
        double x2 = x * x;
        double term = x;
        double sum = x;
        for (int n = 1; n < 10; ++n) {
            term *= -x2 / static_cast<double>((2 * n) * (2 * n + 1));
            sum += term;
        }
        table[i] = sum;
    }
    return table;
}

constexpr auto SIN_TABLE = generate_sin_table<1024>();
```

运行时查表只需一次数组索引 + 一次内存读取，替代了 `std::sin` 的完整浮点计算。

### 编译期字符串哈希

FNV-1a 哈希在编译期求值后可用于 `switch` 语句的 case 标签：

```cpp
constexpr uint64_t fnv1a_hash(const char* str) {
    constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    uint64_t hash = FNV_OFFSET;
    while (*str) {
        hash ^= static_cast<uint64_t>(*str++);
        hash *= FNV_PRIME;
    }
    return hash;
}

// 使用：编译期哈希值作为 case 常量
switch (fnv1a_hash(runtime_string)) {
    case "hello"_hash: /* ... */ break;
    case "world"_hash: /* ... */ break;
}
```

这将字符串比较（O(n)）转化为整数比较（O(1)），且哈希计算本身零运行时开销。

### 验证

```bash
./build/release/examples/03-modern-cpp/compile_time
```

输出中 `Constexpr factorial` 的循环时间应接近零（编译器将 `factorial_constexpr(10)` 折叠为常量 `3628800`），而 `Runtime factorial` 需要实际执行乘法。

---

## 移动语义

对于持有堆内存的对象，移动语义将 O(n) 的深拷贝转化为 O(1) 的指针交换。`examples/03-modern-cpp/src/move_semantics.cpp` 用 1MB Buffer 对象量化这一差异。

### 核心对比

```cpp
// push_back 拷贝：每次分配 1MB + memcpy
for (int i = 0; i < NUM_BUFFERS; ++i) {
    Buffer buf(BUFFER_SIZE, &metrics);
    vec.push_back(buf);  // Copy: 分配 + 复制 1MB
}

// push_back 移动：交换指针，零拷贝
for (int i = 0; i < NUM_BUFFERS; ++i) {
    Buffer buf(BUFFER_SIZE, &metrics);
    vec.push_back(std::move(buf));  // Move: 交换 3 个指针
}

// emplace_back：原地构造，连移动都省了
for (int i = 0; i < NUM_BUFFERS; ++i) {
    vec.emplace_back(BUFFER_SIZE, &metrics);  // 直接在 vector 内存中构造
}
```

代码通过 `OperationMetrics`（`include/hpc/instrumentation.hpp`）精确计数 copy/move 操作次数：

```cpp
class OperationMetrics {
public:
    size_t copy_count = 0;
    size_t move_count = 0;
    size_t allocation_count = 0;
    // ...
};
```

### RVO/NRVO

```cpp
Buffer create_buffer(size_t size, OperationMetrics* metrics) {
    Buffer buf(size, metrics);
    return buf;  // NRVO: 编译器直接在调用方内存中构造
}
```

C++17 起，prvalue 返回保证 copy elision；NRVO（命名返回值优化）在 `-O2` 以上几乎总是生效。输出中 copies 和 moves 应为 0。

### 验证

```bash
./build/release/examples/03-modern-cpp/move_semantics_bench
```

Google Benchmark 输出中，move 和 emplace_back 的耗时应比 copy 低 1-2 个数量级（100 × 1MB 拷贝 ≈ 100MB memcpy）。

---

## 容器容量管理

`std::vector` 的自动增长策略（GCC/Clang 2x，MSVC 1.5x）在不知道最终大小时是合理的默认行为，但在已知大小时造成 O(log N) 次不必要的重分配。

### 增长模式观察

`examples/03-modern-cpp/src/vector_reserve.cpp`：

```cpp
void demonstrate_growth_pattern() {
    std::vector<int> vec;
    size_t last_capacity = 0;
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
        if (vec.capacity() != last_capacity) {
            std::cout << vec.size() << "\t" << vec.capacity() << "\t\tYes\n";
            last_capacity = vec.capacity();
        }
    }
}
```

每次 capacity 变化意味着：分配新内存 → 移动/拷贝所有现有元素 → 释放旧内存。

### 用 CountingAllocator 量化开销

`include/hpc/vector_reserve.hpp` 提供了注入式分配器：

```cpp
template <typename T>
class CountingAllocator {
public:
    T* allocate(std::size_t n) {
        if (metrics_) {
            metrics_->record_allocation(n * sizeof(T));
        }
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }
    // ...
};
```

对比实验（100 万个 int）：

```cpp
// 无 reserve：~20 次分配，累计分配 ~8MB（2x 增长）
std::vector<int, CountingAllocator<int>> vec(alloc);
for (size_t i = 0; i < N; ++i) vec.push_back(i);

// 有 reserve：1 次分配，4MB
vec.reserve(N);
for (size_t i = 0; i < N; ++i) vec.push_back(i);
```

### reserve vs resize

```cpp
vec.reserve(10);  // capacity=10, size=0 — 未初始化，不可下标访问
vec.resize(10);   // capacity≥10, size=10 — 已值初始化，可下标访问
```

`reserve` 用于后续 `push_back`/`emplace_back`；`resize` 用于需要立即按下标访问的场景。

### 验证

```bash
./build/release/examples/03-modern-cpp/vector_reserve_bench
```

---

## C++20 Ranges

Ranges 的核心性能优势不在于单个算法的速度，而在于**惰性求值消除中间分配**。

### 三种实现对比

`include/hpc/ranges_utils.hpp`：

```cpp
// 裸循环：手动管理输出容器
inline void transform_raw_loop(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] * 2 + 1;
    }
}

// std::transform：算法抽象，性能等价
inline void transform_algorithm(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    std::transform(input.begin(), input.end(), output.begin(),
                   [](int x) { return x * 2 + 1; });
}

// ranges::transform：接受整个 range，无需手写 begin/end
inline void transform_ranges(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    std::ranges::transform(input, output.begin(), [](int x) { return x * 2 + 1; });
}
```

对于简单 transform，三者编译后的机器码几乎相同——ranges 的抽象零开销。

### 链式操作的惰性求值

```cpp
// 裸循环：一次遍历，但逻辑耦合
inline std::vector<int> chain_raw_loop(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);
    for (int x : input) {
        if (x % 2 == 0) {
            output.push_back(x * 2 + 1);
        }
    }
    return output;
}

// Ranges view：惰性管道，不产生中间容器
inline auto chain_ranges_view(const std::vector<int>& input) {
    return input
         | std::views::filter([](int x) { return x % 2 == 0; })
         | std::views::transform([](int x) { return x * 2 + 1; });
}
```

`chain_ranges_view` 返回一个 view 对象——不分配内存、不遍历数据。只有在实际迭代（或 `to_vector()` 物化）时才执行计算。如果只需要前 N 个结果，配合 `views::take(N)` 可以提前终止，裸循环做不到这一点。

### 何时选择 Ranges

- **简单遍历/变换**：性能等价，ranges 更简洁
- **多步管道**：ranges view 避免中间 vector 分配
- **需要提前终止**：`views::take` / `views::drop_while` 天然支持
- **需要并行化**：ranges 与 execution policy 的组合（C++23 `std::ranges::for_each` + `std::execution::par`）

### 验证

```bash
./build/release/examples/03-modern-cpp/ranges_vs_loops_bench
```

观察 "Ranges (lazy sum)" 与 "Raw loop" 的耗时对比——对于纯求和场景，惰性 view 无需物化中间结果，性能应持平或略优。

---

## 动手验证

完整 benchmark 命令序列：

```bash
# 构建 Release（确保 -O3 -march=native 生效）
cmake --preset=release
cmake --build build/release

# 编译期计算 demo
./build/release/examples/03-modern-cpp/compile_time

# 移动语义 benchmark
./build/release/examples/03-modern-cpp/move_semantics_bench

# 容器容量管理 benchmark
./build/release/examples/03-modern-cpp/vector_reserve_bench

# Ranges vs 裸循环 benchmark
./build/release/examples/03-modern-cpp/ranges_vs_loops_bench

# 运行所有 benchmark（Google Benchmark 格式，可导出 JSON）
./build/release/examples/03-modern-cpp/move_semantics_bench --benchmark_format=json
./build/release/examples/03-modern-cpp/vector_reserve_bench --benchmark_format=json
./build/release/examples/03-modern-cpp/ranges_vs_loops_bench --benchmark_format=json
```

所有 benchmark 使用 Google Benchmark 框架，自动处理预热、迭代次数自适应和统计显著性。结果因硬件而异——本仓库的原则是提供可运行的验证手段，而非固定的性能数字。
