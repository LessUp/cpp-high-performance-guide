/**
 * @file prefetch.cpp
 * @brief Software prefetching demonstration
 *
 * This example shows how to use __builtin_prefetch to hint the CPU
 * to load data into cache before it's needed. This can improve
 * performance for predictable access patterns on large arrays.
 *
 * Key concepts:
 * - Hardware vs software prefetching
 * - Prefetch distance tuning
 * - When prefetching helps (and when it doesn't)
 *
 * The array traversal routines (sum_no_prefetch / sum_with_prefetch /
 * sum_random_*) live in the canonical library (hpc/memory_utils.hpp) and are
 * shared with bench/prefetch_bench.cpp. The linked-list (pointer chasing)
 * demo below is example-specific.
 */

#include <chrono>
#include <cstdint>
#include <hpc/memory_utils.hpp>
#include <iostream>
#include <random>
#include <vector>

namespace {

//------------------------------------------------------------------------------
// Demonstration
//------------------------------------------------------------------------------

void run_demo() {
    constexpr size_t N = 100'000'000;
    constexpr int ITERATIONS = 5;

    std::cout << "Array size: " << N << " elements (" << (N * sizeof(int64_t) / (1024 * 1024))
              << " MB)\n";
    std::cout << "Iterations: " << ITERATIONS << "\n\n";

    // Initialize data
    std::vector<int64_t> data(N);
    for (size_t i = 0; i < N; ++i) {
        data[i] = static_cast<int64_t>(i % 1000);
    }

    // Sequential access benchmarks
    std::cout << "=== Sequential Access ===\n";

    {
        auto start = std::chrono::steady_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += hpc::memory::sum_no_prefetch(data.data(), N);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "No prefetch:   " << ms << " ms (sum: " << sum << ")\n";
    }

    {
        auto start = std::chrono::steady_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += hpc::memory::sum_with_prefetch(data.data(), N);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "With prefetch: " << ms << " ms (sum: " << sum << ")\n";
    }

    // Random access benchmarks
    std::cout << "\n=== Random Access ===\n";

    std::vector<size_t> indices(N);
    for (size_t i = 0; i < N; ++i) {
        indices[i] = i;
    }

    // Shuffle indices
    std::mt19937 rng(42);
    for (size_t i = N - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        std::swap(indices[i], indices[dist(rng)]);
    }

    {
        auto start = std::chrono::steady_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += hpc::memory::sum_random_no_prefetch(data.data(), indices.data(), N);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "No prefetch:   " << ms << " ms (sum: " << sum << ")\n";
    }

    {
        auto start = std::chrono::steady_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += hpc::memory::sum_random_with_prefetch(data.data(), indices.data(), N);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "With prefetch: " << ms << " ms (sum: " << sum << ")\n";
    }
}

}  // namespace

int main() {
    std::cout << "=== Software Prefetching Demonstration ===\n\n";
    run_demo();

    std::cout << "\nNotes:\n";
    std::cout << "- Sequential access: Hardware prefetcher is very effective,\n";
    std::cout << "  so software prefetching may not help much.\n";
    std::cout << "- Random access: Software prefetching can help by hiding\n";
    std::cout << "  memory latency when access pattern is known ahead of time.\n";

    return 0;
}
