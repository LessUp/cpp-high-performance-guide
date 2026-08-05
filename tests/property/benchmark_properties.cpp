/**
 * @file benchmark_properties.cpp
 * @brief Unit tests for shared benchmark utilities (benchmark_utils.hpp)
 *
 * Feature: hpc-optimization-guide
 * Property 13: Benchmark Utility Functions
 * Validates: Requirements 6.4
 */

#include <gtest/gtest.h>

#include "benchmark_utils.hpp"

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

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
