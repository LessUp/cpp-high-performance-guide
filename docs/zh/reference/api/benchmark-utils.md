# Benchmark 工具 API

用于 benchmark、结果验证与性能测量的通用工具。

---

## 概览

**头文件：** `benchmarks/common/benchmark_utils.hpp`

**命名空间：** `hpc::bench`

---

## 阻止优化

### DoNotOptimize

```cpp
template<typename T>
void DoNotOptimize(T&& value);
```

阻止编译器优化掉某个值。在 benchmark 中不可或缺，用于确保计算确实执行。

**示例：**
```cpp
void BM_vector_push(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        hpc::bench::DoNotOptimize(v.data());  // Prevent optimization
        hpc::bench::ClobberMemory();
    }
}
```

---

### ClobberMemory

```cpp
void ClobberMemory();
```

强制插入内存屏障，阻止编译器重排内存操作。在写入操作之后使用，以确保写入可见。

---

## 结果结构

### BenchmarkResult

```cpp
struct BenchmarkResult {
    std::string name;              // Benchmark name
    std::string module;            // Module name (optional)
    int64_t iterations;            // Number of iterations
    double real_time_ns;           // Wall-clock time (nanoseconds)
    double cpu_time_ns;            // CPU time (nanoseconds)
    double bytes_per_second;       // Memory throughput
    double items_per_second;       // Items processed per second
    std::map<std::string, double> counters;  // Custom counters
    std::string timestamp;         // ISO 8601 timestamp
};
```

单个 benchmark 结果的容器。

---

### BenchmarkSuite

```cpp
struct BenchmarkSuite {
    std::string version;           // Suite version
    std::string compiler;          // Compiler information
    std::string cpu_info;          // CPU information
    std::vector<BenchmarkResult> results;
};
```

带元数据的一组 benchmark 结果集合的容器。

---

## 验证

### validate_result

```cpp
bool validate_result(const BenchmarkResult& result);
```

验证 benchmark 结果是否包含有效值。

**返回值：** 结果有效则返回 `true`，否则返回 `false`

**验证检查项：**
- 名称不为空
- 迭代次数 > 0
- 实际时间 > 0
- CPU 时间 > 0

**示例：**
```cpp
hpc::bench::BenchmarkResult result;
result.name = "vector_push";
result.iterations = 1000;
result.real_time_ns = 50000.0;
result.cpu_time_ns = 48000.0;

if (hpc::bench::validate_result(result)) {
    // Process valid result
}
```

---

## JSON 导出

### export_to_json

```cpp
void export_to_json(
    const std::string& filename,
    const std::vector<BenchmarkResult>& results
);
```

将 benchmark 结果导出为 JSON 格式。

**参数：**
- `filename` - 输出文件路径
- `results` - benchmark 结果向量

**示例：**
```cpp
std::vector<hpc::bench::BenchmarkResult> results;
// ... populate results ...

hpc::bench::export_to_json("benchmark_results.json", results);
```

**输出格式：**
```json
{
  "benchmarks": [
    {
      "name": "BM_vector_push/1024",
      "iterations": 1000,
      "real_time": 50000.00,
      "cpu_time": 48000.00,
      "bytes_per_second": 0,
      "items_per_second": 0,
      "timestamp": "2025-04-23T10:30:00"
    }
  ]
}
```

---

### export_suite_to_json

```cpp
void export_suite_to_json(
    const std::string& filename,
    const BenchmarkSuite& suite
);
```

将完整的 benchmark 套件连同元数据导出为 JSON。

**示例：**
```cpp
hpc::bench::BenchmarkSuite suite;
suite.version = "1.0.0";
suite.compiler = "GCC 13.3.0";
suite.cpu_info = "Intel i7-10700K";
suite.results = { /* ... */ };

hpc::bench::export_suite_to_json("suite_results.json", suite);
```

---

## 工具函数

### calculate_speedup

```cpp
double calculate_speedup(double baseline_time, double optimized_time);
```

计算基线时间与优化时间之间的加速比。

**返回值：** `baseline_time / optimized_time`（值越大越好）

**示例：**
```cpp
double baseline = 1000.0;  // ns
double optimized = 250.0;  // ns
double speedup = hpc::bench::calculate_speedup(baseline, optimized);
// speedup = 4.0x
```

---

### format_throughput

```cpp
std::string format_throughput(double bytes_per_second);
```

将每秒字节数格式化为带适当单位的可读字符串。

**返回值：** 格式化后的字符串，如 "1.50 GB/s"

**示例：**
```cpp
std::string throughput = hpc::bench::format_throughput(1500000000);
// Returns "1.50 GB/s"
```

---

### format_time

```cpp
std::string format_time(double nanoseconds);
```

将纳秒时间格式化为带适当单位的可读字符串。

**返回值：** 格式化后的字符串，如 "1.50 ms"

**示例：**
```cpp
hpc::bench::format_time(500);       // "500.00 ns"
hpc::bench::format_time(5000);      // "5.00 us"
hpc::bench::format_time(5000000);   // "5.00 ms"
hpc::bench::format_time(5000000000);// "5.00 s"
```

---

## Timer 类

### Timer

```cpp
class Timer {
public:
    void start();
    void stop();
    
    double elapsed_ns() const;  // Nanoseconds
    double elapsed_us() const;  // Microseconds
    double elapsed_ms() const;  // Milliseconds
    double elapsed_s() const;   // Seconds
};
```

用于手动计时的简易高分辨率计时器。

**示例：**
```cpp
hpc::bench::Timer timer;

timer.start();
perform_computation();
timer.stop();

std::cout << "Elapsed: " << timer.elapsed_ms() << " ms\n";
std::cout << "Throughput: " 
          << hpc::bench::format_throughput(bytes / timer.elapsed_s())
          << "\n";
```

---

## Google Benchmark 集成

### 与 Google Benchmark 配合使用

```cpp
#include <benchmark/benchmark.h>
#include "benchmark_utils.hpp"

static void BM_example(benchmark::State& state) {
    std::vector<int> data(state.range(0));
    
    for (auto _ : state) {
        // Setup
        std::fill(data.begin(), data.end(), 42);
        
        // Code to benchmark
        long long sum = 0;
        for (int x : data) {
            sum += x;
        }
        
        // Prevent optimization
        hpc::bench::DoNotOptimize(sum);
        hpc::bench::ClobberMemory();
    }
    
    // Set counters
    state.SetBytesProcessed(state.iterations() * state.range(0) * sizeof(int));
    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_example)->Range(1<<10, 1<<20);
```

---

## 快速参考

| 函数 | 用途 |
|----------|---------|
| `DoNotOptimize` | 阻止编译器消除值 |
| `ClobberMemory` | 强制内存屏障 |
| `validate_result` | 检查结果有效性 |
| `export_to_json` | 将结果保存为 JSON |
| `calculate_speedup` | 计算加速比 |
| `format_throughput` | 可读的吞吐量格式 |
| `format_time` | 可读的时间格式 |
| `Timer` | 手动计时类 |

---

## 另请参阅

- [性能分析指南](../../guides/profiling-guide.md)
- [最佳实践](../../guides/best-practices.md)
- [Memory 工具 API](memory-utils.md)
