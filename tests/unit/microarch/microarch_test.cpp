/**
 * @file microarch_test.cpp
 * @brief Correctness tests for the 08-cpu-microarch kernels and graceful
 *        degradation of the hpc::pmu wrapper.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <hpc/pmu.hpp>
#include <random>
#include <vector>

#include "../../../examples/08-cpu-microarch/src/microarch_kernels.hpp"

namespace {

std::vector<int> random_bytes(std::size_t n, unsigned seed) {
    std::vector<int> v(n);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 255);
    for (int& x : v) {
        x = dist(rng);
    }
    return v;
}

}  // namespace

TEST(MicroarchKernelsTest, AllConditionalSumsAgree) {
    const std::vector<int> data = random_bytes(100003, 7);
    const long long branched = microarch::sum_branched(data.data(), data.size(), 128);
    EXPECT_EQ(microarch::sum_ternary(data.data(), data.size(), 128), branched);
    EXPECT_EQ(microarch::sum_masked(data.data(), data.size(), 128), branched);

    // Independent scalar reference.
    long long reference = 0;
    for (int v : data) {
        if (v >= 128) {
            reference += v;
        }
    }
    EXPECT_EQ(branched, reference);
}

TEST(MicroarchKernelsTest, ChainKernelsAreFiniteAndDeterministic) {
    EXPECT_DOUBLE_EQ(microarch::dep_add_chain(1.0, 10000), microarch::dep_add_chain(1.0, 10000));
    EXPECT_DOUBLE_EQ(microarch::ilp4_add(1.0, 10000), microarch::ilp4_add(1.0, 10000));
    EXPECT_GT(microarch::dep_add_chain(1.0, 10000), 1.0);
    EXPECT_GT(microarch::dep_mul_chain(1.0, 10000), 1.0);
    EXPECT_GT(microarch::ilp4_mul(1.0, 10000), 1.0);
}

#if defined(__linux__)
TEST(PmuTest, DegradesGracefullyOrCountsCycles) {
    if (!hpc::pmu::available()) {
        // Unavailable is a valid environment state (WSL2/VM/paranoid); the
        // contract is graceful degradation, not failure.
        hpc::pmu::Counter counter = hpc::pmu::Counter::try_open(hpc::pmu::Event::CpuCycles);
        EXPECT_FALSE(counter.valid());
        return;
    }

    volatile unsigned long long sink = 0;
    const hpc::pmu::Sample sample = hpc::pmu::measure([&] {
        for (int i = 0; i < 1'000'000; ++i) {
            sink += i;
        }
    });
    ASSERT_TRUE(sample.available);
    EXPECT_GT(sample.cpu_cycles, 0u);
    EXPECT_GT(sample.instructions, 0u);
}
#endif
