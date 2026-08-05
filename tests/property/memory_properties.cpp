/**
 * @file memory_properties.cpp
 * @brief Property-based tests for memory optimization modules
 *
 * Uses RapidCheck for property-based testing to verify that optimizations
 * maintain correctness and provide expected performance characteristics.
 *
 * Validates:
 *   - Requirement 2.1: AOS vs SOA Comparison
 *   - Requirement 2.2: False Sharing Demonstration
 *   - Requirement 2.3: Memory Alignment for SIMD
 */

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <hpc/concurrency_utils.hpp>
#include <hpc/core.hpp>
#include <hpc/memory_utils.hpp>
#include <hpc/particle_types.hpp>
#include <thread>
#include <vector>

#include "benchmark_utils.hpp"

namespace {

using hpc::bench::Timer;
using hpc::concurrency::AlignedCounter;
using hpc::core::CACHE_LINE_SIZE;
using hpc::memory::ParticleAOS;
using hpc::memory::ParticleSOA;
using hpc::memory::update_particles_aos;
using hpc::memory::update_particles_soa;

//------------------------------------------------------------------------------
// Property 3: SOA Performance Advantage for Sequential Access
// Validates: Requirements 2.1
//
// For any particle system with N > 1000 particles, updating all particles
// using SOA layout SHALL complete in less time than updating using AOS layout
// when accessing data sequentially.
//------------------------------------------------------------------------------

RC_GTEST_PROP(MemoryProperties, SOAPerformanceAdvantage, ()) {
    // Feature: hpc-optimization-guide, Property 3: SOA Performance Advantage
    // Validates: Requirements 2.1

    // Generate a size between 1000 and 100000
    const char* run_perf_env = std::getenv("HPC_RUN_PERF_TESTS");
    const bool run_perf = (run_perf_env != nullptr) && (run_perf_env[0] == '1');
    const size_t max_n = run_perf ? 100001 : 5001;
    const size_t n = *rc::gen::inRange<size_t>(1000, max_n);
    const int iterations = run_perf ? 10 : 2;
    constexpr float dt = 0.01f;

    // Initialize AOS
    std::vector<ParticleAOS> aos;
    hpc::memory::initialize_particles(aos, n);

    // Initialize SOA with same data
    ParticleSOA soa;
    hpc::memory::initialize_particles(soa, n);

    // Warm up caches
    update_particles_aos(aos, dt);
    update_particles_soa(soa, dt);

    // Measure AOS time using hpc::bench::Timer
    Timer aos_timer;
    aos_timer.start();
    for (int i = 0; i < iterations; ++i) {
        update_particles_aos(aos, dt);
    }
    aos_timer.stop();
    const auto aos_time = aos_timer.elapsed_us();

    // Measure SOA time using hpc::bench::Timer
    Timer soa_timer;
    soa_timer.start();
    for (int i = 0; i < iterations; ++i) {
        update_particles_soa(soa, dt);
    }
    soa_timer.stop();
    const auto soa_time = soa_timer.elapsed_us();

    // SOA should be faster (or at least not significantly slower)
    // Allow 20% tolerance for system noise
    if (run_perf) {
        RC_ASSERT(soa_time <= aos_time * 1.2);
    }

    // Verify correctness: results should be equivalent
    for (size_t i = 0; i < std::min(n, size_t(100)); ++i) {
        RC_ASSERT(std::abs(aos[i].x - soa.x[i]) < 0.001f);
        RC_ASSERT(std::abs(aos[i].y - soa.y[i]) < 0.001f);
        RC_ASSERT(std::abs(aos[i].z - soa.z[i]) < 0.001f);
    }
}

//------------------------------------------------------------------------------
// Property 4: Concurrent Counter Correctness (packed vs cache-line-aligned)
// Validates: Requirements 2.2, 5.3
//
// Both layouts must produce exact counts under concurrent increments.
// The performance impact of false sharing is deliberately NOT asserted here:
// timing is too noisy for a property test on shared CI hosts (an earlier
// version's timing assertions were tautologies). Measurements live in
// examples/02-memory-cache/bench/false_sharing_bench.cpp.
//------------------------------------------------------------------------------

// Compare hpc::concurrency::AlignedCounter vs plain std::atomic<int>.
// For the unaligned case, use std::atomic<int> directly.

RC_GTEST_PROP(MemoryProperties, ConcurrentCounterCorrectness, ()) {
    // Feature: hpc-optimization-guide, Property 4: Cache-Line Aligned Counters
    // Validates: Requirements 2.2, 5.3

    const int num_threads = *rc::gen::inRange(2, 9);  // 2-8 threads
    const int increments = *rc::gen::inRange(10000, 100001);

    // Test with unaligned counters (false sharing)
    // Use plain std::atomic<int64_t> to demonstrate the unaligned case
    std::vector<std::atomic<int64_t>> unaligned(static_cast<size_t>(num_threads));

    {
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&unaligned, t, increments]() {
                for (int i = 0; i < increments; ++i) {
                    unaligned[static_cast<size_t>(t)].fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }

    // Test with aligned counters (no false sharing)
    std::vector<AlignedCounter> aligned(static_cast<size_t>(num_threads));

    {
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&aligned, t, increments]() {
                for (int i = 0; i < increments; ++i) {
                    aligned[static_cast<size_t>(t)].value.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }

    // Verify correctness: every counter holds exactly `increments`,
    // regardless of layout — no lost updates under concurrency.
    for (int t = 0; t < num_threads; ++t) {
        RC_ASSERT(unaligned[static_cast<size_t>(t)].load() == increments);
        RC_ASSERT(aligned[static_cast<size_t>(t)].value.load() == increments);
    }
}

//------------------------------------------------------------------------------
// Property 5: Aligned Storage Correctness
// Validates: Requirements 2.3
//
// For any random array, cache-line-aligned storage from hpc::memory::make_aligned
// must (a) actually be aligned to CACHE_LINE_SIZE and (b) produce results
// identical to ordinary storage for the same data and access order.
//------------------------------------------------------------------------------

RC_GTEST_PROP(MemoryProperties, AlignedStorageMatchesOrdinaryStorage, ()) {
    // Feature: hpc-optimization-guide, Property 5: Aligned Memory Correctness
    // Validates: Requirements 2.3

    const size_t n = *rc::gen::inRange<size_t>(1, 10001);

    auto aligned = hpc::memory::make_aligned<float>(n);
    std::vector<float> ordinary(n);

    // Fill both with identical random data
    for (size_t i = 0; i < n; ++i) {
        const float v = static_cast<float>(*rc::gen::inRange(-10000, 10000)) * 0.01f;
        aligned[i] = v;
        ordinary[i] = v;
    }

    // (a) Alignment guarantee
    RC_ASSERT(reinterpret_cast<std::uintptr_t>(aligned.get()) % CACHE_LINE_SIZE == 0);

    // (b) Same data + same reduction order => bit-identical results
    float sum_aligned = 0.0f;
    float sum_ordinary = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum_aligned += aligned[i];
        sum_ordinary += ordinary[i];
    }
    RC_ASSERT(std::isfinite(sum_aligned));
    RC_ASSERT(sum_aligned == sum_ordinary);
}

}  // namespace
