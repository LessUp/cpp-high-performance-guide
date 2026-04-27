# 基准测试工具 API

用于基准测试、结果验证和性能测量的通用工具。

---

## 概述

**头文件：** `benchmarks/common/benchmark_utils.hpp`

**命名空间：** `hpc::bench`

---

## 防止优化

### DoNotOptimize

```cpp
template<typename T>
void DoNotOptimize(T&& value);
```

防止编译器优化掉某个值。在基准测试中至关重要，确保计算确实执行。

**示例：**
```cpp
void BM_vector_push(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        hpc::bench::DoNotOptimize(v.data());  // 防止优化
        hpc::bench::ClobberMemory();
    }
}
```

---

### ClobberMemory

```cpp
void ClobberMemory();
```

强制内存屏障，防止编译器重排内存操作。在写入后使用，确保写入可见。

---

## 结果结构

### BenchmarkResult

```cpp
struct BenchmarkResult {
    std::string name;              // 基准测试名称
    std::string module;            // 模块名称（可选）
    int64_t iterations;            // 迭代次数
    double real_time_ns;           // 墙上时钟时间（纳秒）
    double cpu_time_ns;            // CPU 时间（纳秒）
    double bytes_per_second;       // 内存吞吐量
    double items_per_second;       // 每秒处理项数
    std::map<std::string, double> counters;  // 自定义计数器
    std::string timestamp;         // ISO 8601 时间戳
};
```

单个基准测试结果的容器。

---

### BenchmarkSuite

```cpp
struct BenchmarkSuite {
    std::string version;           // 套件版本
    std::string compiler;          // 编译器信息
    std::string cpu_info;          // CPU 信息
    std::vector<BenchmarkResult> results;
};
```

包含元数据的基准测试结果集合容器。

---

## 验证

### validate_result

```cpp
bool validate_result(const BenchmarkResult& result);
```

验证基准测试结果具有有效值。

**返回：** 如果结果有效返回 `true`，否则返回 `false`

**验证检查：**
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
    // 处理有效结果
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

将基准测试结果导出为 JSON 格式。

**参数：**
- `filename` - 输出文件路径
- `results` - 基准测试结果向量

**示例：**
```cpp
std::vector<hpc::bench::BenchmarkResult> results;
// ... 填充结果 ...

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

将包含元数据的完整基准测试套件导出为 JSON。

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

计算基准时间和优化时间之间的加速比。

**返回：** `baseline_time / optimized_time`（越高越好）

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

将每秒字节数格式化为带适当单位的人类可读字符串。

**返回：** 格式化字符串如 "1.50 GB/s"

**示例：**
```cpp
std::string throughput = hpc::bench::format_throughput(1500000000);
// 返回 "1.50 GB/s"
```

---

### format_time

```cpp
std::string format_time(double nanoseconds);
```

将纳秒时间格式化为带适当单位的人类可读字符串。

**返回：** 格式化字符串如 "1.50 ms"

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
    
    double elapsed_ns() const;  // 纳秒
    double elapsed_us() const;  // 微秒
    double elapsed_ms() const;  // 毫秒
    double elapsed_s() const;   // 秒
};
```

简单的高精度计时器，用于手动计时。

**示例：**
```cpp
hpc::bench::Timer timer;

timer.start();
perform_computation();
timer.stop();

std::cout << "耗时：" << timer.elapsed_ms() << " ms\n";
std::cout << "吞吐量：" 
          << hpc::bench::format_throughput(bytes / timer.elapsed_s())
          << "\n";
```

---

## Google Benchmark 集成

### 配合 Google Benchmark 使用

```cpp
#include <benchmark/benchmark.h>
#include "benchmark_utils.hpp"

static void BM_example(benchmark::State& state) {
    std::vector<int> data(state.range(0));
    
    for (auto _ : state) {
        // 设置
        std::fill(data.begin(), data.end(), 42);
        
        // 待测试的代码
        long long sum = 0;
        for (int x : data) {
            sum += x;
        }
        
        // 防止优化
        hpc::bench::DoNotOptimize(sum);
        hpc::bench::ClobberMemory();
    }
    
    // 设置计数器
    state.SetBytesProcessed(state.iterations() * state.range(0) * sizeof(int));
    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_example)->Range(1<<10, 1<<20);
```

---

## 快速参考

| 函数 | 用途 |
|------|------|
| `DoNotOptimize` | 防止编译器消除值 |
| `ClobberMemory` | 强制内存屏障 |
| `validate_result` | 检查结果有效性 |
| `export_to_json` | 保存结果到 JSON |
| `calculate_speedup` | 计算加速比 |
| `format_throughput` | 人类可读的吞吐量 |
| `format_time` | 人类可读的时间 |
| `Timer` | 手动计时类 |

---

## 另见

- [性能分析指南](../../guides/profiling-guide.md)
- [最佳实践](../../guides/best-practices.md)
- [内存工具 API](memory-utils.md)
