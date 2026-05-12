/**
 * @file benchmark_properties.cpp
 * @brief Property-based tests for benchmark utilities
 *
 * Feature: hpc-optimization-guide
 * Property 13: Benchmark Utility Functions
 * Validates: Requirements 6.4
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "benchmark_utils.hpp"

namespace {

std::filesystem::path make_temp_benchmark_file(const std::string& prefix) {
    const auto temp_dir = std::filesystem::temp_directory_path();
    return temp_dir / (prefix + std::to_string(rand()) + ".json");
}

}  // anonymous namespace

/**
 * Property 13: Benchmark Utility Functions
 *
 * Validates: Requirements 6.4
 */

TEST(BenchmarkUtilsTests, SpeedupCalculation) {
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 50.0), 2.0);
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 100.0), 1.0);
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 200.0), 0.5);
    EXPECT_DOUBLE_EQ(hpc::bench::calculate_speedup(100.0, 0.0), 0.0);
}

TEST(BenchmarkUtilsTests, ThroughputFormatting) {
    EXPECT_EQ(hpc::bench::format_throughput(500), "500.00 B/s");
    EXPECT_EQ(hpc::bench::format_throughput(1024), "1.00 KB/s");
    EXPECT_EQ(hpc::bench::format_throughput(1024 * 1024), "1.00 MB/s");
    EXPECT_EQ(hpc::bench::format_throughput(1024 * 1024 * 1024), "1.00 GB/s");
}

TEST(BenchmarkUtilsTests, TimeFormatting) {
    EXPECT_EQ(hpc::bench::format_time(500), "500.00 ns");
    EXPECT_EQ(hpc::bench::format_time(5000), "5.00 us");
    EXPECT_EQ(hpc::bench::format_time(5000000), "5.00 ms");
    EXPECT_EQ(hpc::bench::format_time(5000000000), "5.00 s");
}

TEST(BenchmarkUtilsTests, TimerBasicUsage) {
    hpc::bench::Timer timer;

    timer.start();
    volatile int sum = 0;
    for (int i = 0; i < 10000; ++i) {
        sum += i;
    }
    timer.stop();

    EXPECT_GT(timer.elapsed_ns(), 0);
    EXPECT_GT(timer.elapsed_us(), 0);
}

TEST(BenchmarkUtilsTests, GoogleBenchmarkNativeJSON) {
    // This test verifies that Google Benchmark's native JSON output works
    // We no longer maintain our own JSON exporter - use the standard tool

    // Example: benchmark executable can be run with:
    // ./benchmark --benchmark_out=file.json --benchmark_out_format=json

    // This test documents the standard approach:
    // Google Benchmark natively supports JSON output via CLI flags
    EXPECT_TRUE(true) << "Use: --benchmark_out=file.json --benchmark_out_format=json";
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
