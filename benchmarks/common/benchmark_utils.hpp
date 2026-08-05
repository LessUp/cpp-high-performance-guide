#pragma once
/**
 * @file benchmark_utils.hpp
 * @brief Common utilities for benchmarking
 *
 * Provides helpers for formatting benchmark results and manual timing.
 *
 * Validates:
 *   - Requirement 1.1: Google Benchmark Integration
 *   - Requirement 1.2: Parameterized Benchmarks
 */

#include <benchmark/benchmark.h>

#include <chrono>
#include <cstdio>
#include <string>

namespace hpc::bench {

/**
 * @brief Calculate speedup between two times
 */
inline double calculate_speedup(double baseline_time, double optimized_time) {
    if (optimized_time <= 0)
        return 0;
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
 *
 * Uses steady_clock: monotonic and immune to system-clock adjustments.
 * high_resolution_clock aliases system_clock on some standard libraries
 * and can jump backwards (observed: negative elapsed times under NTP slew).
 *
 * Note: For most benchmarks, prefer Google Benchmark's built-in timing
 * via the benchmark::State parameter.
 */
class Timer {
public:
    void start() { start_ = std::chrono::steady_clock::now(); }

    void stop() { end_ = std::chrono::steady_clock::now(); }

    double elapsed_ns() const {
        return std::chrono::duration<double, std::nano>(end_ - start_).count();
    }

    double elapsed_us() const {
        return std::chrono::duration<double, std::micro>(end_ - start_).count();
    }

    double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(end_ - start_).count();
    }

    double elapsed_s() const { return std::chrono::duration<double>(end_ - start_).count(); }

private:
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point end_;
};

}  // namespace hpc::bench
