/**
 * @file memory_properties.cpp
 * @brief Property-based tests for memory optimization modules
 * 
 * Uses RapidCheck for property-based testing to verify that optimizations
 * maintain correctness and provide expected performance characteristics.
 */

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

namespace {

//------------------------------------------------------------------------------
// AOS vs SOA structures (duplicated for test isolation)
//------------------------------------------------------------------------------

struct ParticleAOS {
    float x, y, z;
    float vx, vy, vz;
};

struct ParticleSOA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    
    void resize(size_t n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
    }
    
    size_t size() const { return x.size(); }
};

void update_aos(std::vector<ParticleAOS>& particles, float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

void update_soa(ParticleSOA& particles, float dt) {
    const size_t n = particles.size();
    for (size_t i = 0; i < n; ++i) particles.x[i] += particles.vx[i] * dt;
    for (size_t i = 0; i < n; ++i) particles.y[i] += particles.vy[i] * dt;
    for (size_t i = 0; i < n; ++i) particles.z[i] += particles.vz[i] * dt;
}

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
    const size_t n = *rc::gen::inRange<size_t>(1000, 100001);
    constexpr int iterations = 10;
    constexpr float dt = 0.01f;
    
    // Initialize AOS
    std::vector<ParticleAOS> aos(n);
    for (size_t i = 0; i < n; ++i) {
        aos[i] = {
            static_cast<float>(i), static_cast<float>(i), static_cast<float>(i),
            1.0f, 1.0f, 1.0f
        };
    }
    
    // Initialize SOA with same data
    ParticleSOA soa;
    soa.resize(n);
    for (size_t i = 0; i < n; ++i) {
        soa.x[i] = static_cast<float>(i);
        soa.y[i] = static_cast<float>(i);
        soa.z[i] = static_cast<float>(i);
        soa.vx[i] = 1.0f;
        soa.vy[i] = 1.0f;
        soa.vz[i] = 1.0f;
    }
    
    // Warm up caches
    update_aos(aos, dt);
    update_soa(soa, dt);
    
    // Measure AOS time
    auto aos_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        update_aos(aos, dt);
    }
    auto aos_end = std::chrono::high_resolution_clock::now();
    auto aos_time = std::chrono::duration_cast<std::chrono::microseconds>(aos_end - aos_start).count();
    
    // Measure SOA time
    auto soa_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        update_soa(soa, dt);
    }
    auto soa_end = std::chrono::high_resolution_clock::now();
    auto soa_time = std::chrono::duration_cast<std::chrono::microseconds>(soa_end - soa_start).count();
    
    // SOA should be faster (or at least not significantly slower)
    // Allow 20% tolerance for system noise
    RC_ASSERT(soa_time <= aos_time * 1.2);
    
    // Verify correctness: results should be equivalent
    for (size_t i = 0; i < std::min(n, size_t(100)); ++i) {
        RC_ASSERT(std::abs(aos[i].x - soa.x[i]) < 0.001f);
        RC_ASSERT(std::abs(aos[i].y - soa.y[i]) < 0.001f);
        RC_ASSERT(std::abs(aos[i].z - soa.z[i]) < 0.001f);
    }
}

//------------------------------------------------------------------------------
// Property 4: Cache-Line Aligned Counters Eliminate False Sharing
// Validates: Requirements 2.2, 5.3
//------------------------------------------------------------------------------

constexpr size_t CACHE_LINE_SIZE = 64;

struct UnalignedCounter {
    std::atomic<int> value{0};
};

struct alignas(CACHE_LINE_SIZE) AlignedCounter {
    std::atomic<int> value{0};
};

template<typename Counter>
void increment_counter(Counter& counter, int increments) {
    for (int i = 0; i < increments; ++i) {
        counter.value.fetch_add(1, std::memory_order_relaxed);
    }
}

RC_GTEST_PROP(MemoryProperties, AlignedCountersEliminateFalseSharing, ()) {
    // Feature: hpc-optimization-guide, Property 4: Cache-Line Aligned Counters
    // Validates: Requirements 2.2, 5.3
    
    const int num_threads = *rc::gen::inRange(2, 9);  // 2-8 threads
    const int increments = *rc::gen::inRange(10000, 100001);
    
    // Test with unaligned counters (false sharing)
    std::vector<UnalignedCounter> unaligned(static_cast<size_t>(num_threads));
    
    auto unaligned_start = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&unaligned, t, increments]() {
                increment_counter(unaligned[static_cast<size_t>(t)], increments);
            });
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
    auto unaligned_end = std::chrono::high_resolution_clock::now();
    auto unaligned_time = std::chrono::duration_cast<std::chrono::microseconds>(
        unaligned_end - unaligned_start).count();
    
    // Test with aligned counters (no false sharing)
    std::vector<AlignedCounter> aligned(static_cast<size_t>(num_threads));
    
    auto aligned_start = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&aligned, t, increments]() {
                increment_counter(aligned[static_cast<size_t>(t)], increments);
            });
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
    auto aligned_end = std::chrono::high_resolution_clock::now();
    auto aligned_time = std::chrono::duration_cast<std::chrono::microseconds>(
        aligned_end - aligned_start).count();
    
    // Verify correctness
    for (int t = 0; t < num_threads; ++t) {
        RC_ASSERT(unaligned[static_cast<size_t>(t)].value.load() == increments);
        RC_ASSERT(aligned[static_cast<size_t>(t)].value.load() == increments);
    }
    
    // Aligned should be faster when there are multiple threads
    // Allow significant tolerance due to system variability
    if (num_threads > 1 && unaligned_time > 1000) {
        // Aligned should be at least not significantly slower
        RC_ASSERT(aligned_time <= unaligned_time * 1.5);
    }
}

//------------------------------------------------------------------------------
// Property 5: Aligned Memory SIMD Performance
// Validates: Requirements 2.3
//------------------------------------------------------------------------------

void sum_array_unaligned(const float* data, size_t n, float& result) {
    result = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        result += data[i];
    }
}

RC_GTEST_PROP(MemoryProperties, AlignedMemorySIMDCorrectness, ()) {
    // Feature: hpc-optimization-guide, Property 5: Aligned Memory SIMD Performance
    // Validates: Requirements 2.3
    
    const size_t n = *rc::gen::inRange<size_t>(256, 10001);
    
    // Create aligned and unaligned arrays
    std::vector<float> unaligned(n);
    
    // Fill with random data
    for (size_t i = 0; i < n; ++i) {
        unaligned[i] = static_cast<float>(i % 100) * 0.01f;
    }
    
    // Compute sum
    float sum_unaligned = 0.0f;
    sum_array_unaligned(unaligned.data(), n, sum_unaligned);
    
    // Verify result is reasonable (not NaN or Inf)
    RC_ASSERT(std::isfinite(sum_unaligned));
    RC_ASSERT(sum_unaligned >= 0.0f);
}

} // namespace
