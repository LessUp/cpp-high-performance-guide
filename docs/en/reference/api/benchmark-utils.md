# Benchmark Utilities API

Common utilities for benchmarking, result validation, and performance measurement.

---

## Overview

**Header:** `benchmarks/common/benchmark_utils.hpp`

**Namespace:** `hpc::bench`

---

## Optimization Prevention

### DoNotOptimize

```cpp
template<typename T>
void DoNotOptimize(T&& value);
```

Prevent the compiler from optimizing away a value. Essential for benchmarking to ensure computations actually execute.

**Example:**
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

Force a memory barrier, preventing the compiler from reordering memory operations. Use after writes to ensure they are visible.

---

## Result Structures

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

Container for a single benchmark result.

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

Container for a collection of benchmark results with metadata.

---

## Validation

### validate_result

```cpp
bool validate_result(const BenchmarkResult& result);
```

Validate that a benchmark result has valid values.

**Returns:** `true` if result is valid, `false` otherwise

**Validation checks:**
- Name is not empty
- Iterations > 0
- Real time > 0
- CPU time > 0

**Example:**
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

## JSON Export

### export_to_json

```cpp
void export_to_json(
    const std::string& filename,
    const std::vector<BenchmarkResult>& results
);
```

Export benchmark results to JSON format.

**Parameters:**
- `filename` - Output file path
- `results` - Vector of benchmark results

**Example:**
```cpp
std::vector<hpc::bench::BenchmarkResult> results;
// ... populate results ...

hpc::bench::export_to_json("benchmark_results.json", results);
```

**Output format:**
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

Export a complete benchmark suite with metadata to JSON.

**Example:**
```cpp
hpc::bench::BenchmarkSuite suite;
suite.version = "1.0.0";
suite.compiler = "GCC 13.3.0";
suite.cpu_info = "Intel i7-10700K";
suite.results = { /* ... */ };

hpc::bench::export_suite_to_json("suite_results.json", suite);
```

---

## Utility Functions

### calculate_speedup

```cpp
double calculate_speedup(double baseline_time, double optimized_time);
```

Calculate speedup ratio between baseline and optimized times.

**Returns:** `baseline_time / optimized_time` (higher is better)

**Example:**
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

Format bytes per second as human-readable string with appropriate units.

**Returns:** Formatted string like "1.50 GB/s"

**Example:**
```cpp
std::string throughput = hpc::bench::format_throughput(1500000000);
// Returns "1.50 GB/s"
```

---

### format_time

```cpp
std::string format_time(double nanoseconds);
```

Format time in nanoseconds as human-readable string with appropriate units.

**Returns:** Formatted string like "1.50 ms"

**Example:**
```cpp
hpc::bench::format_time(500);       // "500.00 ns"
hpc::bench::format_time(5000);      // "5.00 us"
hpc::bench::format_time(5000000);   // "5.00 ms"
hpc::bench::format_time(5000000000);// "5.00 s"
```

---

## Timer Class

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

Simple high-resolution timer for manual timing.

**Example:**
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

## Google Benchmark Integration

### Using with Google Benchmark

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

## Quick Reference

| Function | Purpose |
|----------|---------|
| `DoNotOptimize` | Prevent compiler from eliding values |
| `ClobberMemory` | Force memory barrier |
| `validate_result` | Check result validity |
| `export_to_json` | Save results to JSON |
| `calculate_speedup` | Compute speedup ratio |
| `format_throughput` | Human-readable throughput |
| `format_time` | Human-readable time |
| `Timer` | Manual timing class |

---

## See Also

- [Profiling Guide](../../guides/profiling-guide.md)
- [Best Practices](../../guides/best-practices.md)
- [Memory Utilities API](memory-utils.md)
