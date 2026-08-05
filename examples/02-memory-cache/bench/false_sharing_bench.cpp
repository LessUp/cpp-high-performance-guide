/**
 * @file false_sharing_bench.cpp
 * @brief Benchmark for false sharing demonstration
 *
 * Property 4: Cache-Line Aligned Counters Eliminate False Sharing
 * Validates: Requirements 2.2, 5.3
 *
 * Both variants use exactly one counter per thread, so the ONLY difference
 * between the two measurements is the memory layout: packed counters share
 * cache lines (false sharing), padded counters (hpc::concurrency::AlignedCounter
 * from the canonical library) each occupy their own cache line.
 */

#include <benchmark/benchmark.h>

#include <atomic>
#include <hpc/concurrency_utils.hpp>
#include <thread>
#include <vector>

namespace {

// Packed counters: contiguous atomics, several per cache line → false sharing.
void increment_packed(std::vector<std::atomic<int64_t>>& counters, int id, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        counters[static_cast<size_t>(id)].fetch_add(1, std::memory_order_relaxed);
    }
}

// Padded counters: one cache line each → no false sharing.
void increment_padded(std::vector<hpc::concurrency::AlignedCounter>& counters, int id, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        counters[static_cast<size_t>(id)].increment(std::memory_order_relaxed);
    }
}

static void BM_FalseSharing_Packed(benchmark::State& state) {
    const int num_threads = static_cast<int>(state.range(0));
    const int64_t iterations = 100000;

    for (auto _ : state) {
        std::vector<std::atomic<int64_t>> counters(static_cast<size_t>(num_threads));
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back(increment_packed, std::ref(counters), t, iterations);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        benchmark::DoNotOptimize(counters.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * num_threads * iterations);
}

static void BM_FalseSharing_Padded(benchmark::State& state) {
    const int num_threads = static_cast<int>(state.range(0));
    const int64_t iterations = 100000;

    for (auto _ : state) {
        std::vector<hpc::concurrency::AlignedCounter> counters(static_cast<size_t>(num_threads));
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back(increment_padded, std::ref(counters), t, iterations);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        benchmark::DoNotOptimize(counters.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * num_threads * iterations);
}

BENCHMARK(BM_FalseSharing_Packed)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_FalseSharing_Padded)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

}  // namespace
