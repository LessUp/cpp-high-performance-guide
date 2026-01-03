#pragma once
/**
 * @file benchmark_utils.hpp
 * @brief Common utilities for benchmarking
 * 
 * Provides helpers for preventing compiler optimizations and
 * exporting benchmark results.
 */

#include <benchmark/benchmark.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace hpc::bench {

/**
 * @brief Prevent compiler from optimizing away a value
 * 
 * This is a wrapper around Google Benchmark's DoNotOptimize.
 */
template<typename T>
inline void DoNotOptimize(T&& value) {
    benchmark::DoNotOptimize(std::forward<T>(value));
}

/**
 * @brief Force memory barrier
 * 
 * Prevents compiler from reordering memory operations.
 */
inline void ClobberMemory() {
    benchmark::ClobberMemory();
}

/**
 * @brief Benchmark result structure
 */
struct BenchmarkResult {
    std::string name;
    std::string module;
    int64_t iterations;
    double real_time_ns;
    double cpu_time_ns;
    double bytes_per_second;
    double items_per_second;
    std::map<std::string, double> counters;
    std::string timestamp;
    
    BenchmarkResult() = default;
    
    BenchmarkResult(const std::string& n, int64_t iter, double real_ns, double cpu_ns)
        : name(n), iterations(iter), real_time_ns(real_ns), cpu_time_ns(cpu_ns),
          bytes_per_second(0), items_per_second(0) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
        timestamp = ss.str();
    }
};

/**
 * @brief Benchmark suite containing multiple results
 */
struct BenchmarkSuite {
    std::string version;
    std::string compiler;
    std::string cpu_info;
    std::vector<BenchmarkResult> results;
    
    BenchmarkSuite() : version("1.0.0") {}
};

/**
 * @brief Validate benchmark result
 */
inline bool validate_result(const BenchmarkResult& result) {
    if (result.name.empty()) return false;
    if (result.iterations <= 0) return false;
    if (result.real_time_ns <= 0) return false;
    if (result.cpu_time_ns <= 0) return false;
    return true;
}

/**
 * @brief Export benchmark results to JSON
 * 
 * Note: Google Benchmark already supports JSON output via --benchmark_out=file.json
 * This is a simplified custom exporter for demonstration.
 */
inline void export_to_json(const std::string& filename, 
                           const std::vector<BenchmarkResult>& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file << "{\n";
    file << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        file << "    {\n";
        file << "      \"name\": \"" << r.name << "\",\n";
        if (!r.module.empty()) {
            file << "      \"module\": \"" << r.module << "\",\n";
        }
        file << "      \"iterations\": " << r.iterations << ",\n";
        file << "      \"real_time\": " << std::fixed << std::setprecision(2) << r.real_time_ns << ",\n";
        file << "      \"cpu_time\": " << std::fixed << std::setprecision(2) << r.cpu_time_ns << ",\n";
        file << "      \"bytes_per_second\": " << r.bytes_per_second << ",\n";
        file << "      \"items_per_second\": " << r.items_per_second;
        
        if (!r.counters.empty()) {
            file << ",\n      \"counters\": {\n";
            size_t counter_idx = 0;
            for (const auto& [key, value] : r.counters) {
                file << "        \"" << key << "\": " << value;
                if (++counter_idx < r.counters.size()) file << ",";
                file << "\n";
            }
            file << "      }";
        }
        
        if (!r.timestamp.empty()) {
            file << ",\n      \"timestamp\": \"" << r.timestamp << "\"";
        }
        
        file << "\n    }";
        if (i < results.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
}

/**
 * @brief Export benchmark suite to JSON
 */
inline void export_suite_to_json(const std::string& filename,
                                  const BenchmarkSuite& suite) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file << "{\n";
    file << "  \"version\": \"" << suite.version << "\",\n";
    if (!suite.compiler.empty()) {
        file << "  \"compiler\": \"" << suite.compiler << "\",\n";
    }
    if (!suite.cpu_info.empty()) {
        file << "  \"cpu_info\": \"" << suite.cpu_info << "\",\n";
    }
    file << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < suite.results.size(); ++i) {
        const auto& r = suite.results[i];
        file << "    {\n";
        file << "      \"name\": \"" << r.name << "\",\n";
        file << "      \"iterations\": " << r.iterations << ",\n";
        file << "      \"real_time\": " << r.real_time_ns << ",\n";
        file << "      \"cpu_time\": " << r.cpu_time_ns << "\n";
        file << "    }";
        if (i < suite.results.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
}

/**
 * @brief Calculate speedup between two times
 */
inline double calculate_speedup(double baseline_time, double optimized_time) {
    if (optimized_time <= 0) return 0;
    return baseline_time / optimized_time;
}

/**
 * @brief Format bytes per second as human-readable string
 */
inline std::string format_throughput(double bytes_per_second) {
    const char* units[] = {"B/s", "KB/s", "MB/s", "GB/s", "TB/s"};
    int unit_index = 0;
    
    while (bytes_per_second >= 1024 && unit_index < 4) {
        bytes_per_second /= 1024;
        ++unit_index;
    }
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f %s", bytes_per_second, units[unit_index]);
    return buffer;
}

/**
 * @brief Format time in nanoseconds as human-readable string
 */
inline std::string format_time(double nanoseconds) {
    if (nanoseconds < 1000) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f ns", nanoseconds);
        return buffer;
    } else if (nanoseconds < 1000000) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f us", nanoseconds / 1000);
        return buffer;
    } else if (nanoseconds < 1000000000) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f ms", nanoseconds / 1000000);
        return buffer;
    } else {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f s", nanoseconds / 1000000000);
        return buffer;
    }
}

/**
 * @brief Simple timer for manual benchmarking
 */
class Timer {
public:
    void start() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        end_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_ns() const {
        return std::chrono::duration<double, std::nano>(end_ - start_).count();
    }
    
    double elapsed_us() const {
        return std::chrono::duration<double, std::micro>(end_ - start_).count();
    }
    
    double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(end_ - start_).count();
    }
    
    double elapsed_s() const {
        return std::chrono::duration<double>(end_ - start_).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
};

} // namespace hpc::bench
