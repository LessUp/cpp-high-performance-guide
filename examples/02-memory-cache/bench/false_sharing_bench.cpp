/**
 * @file false_sharing_bench.cpp
 * @brief Benchmark for false sharing demonstration
 * 
 * Property 4: Cache-Line Aligned Counters Eliminate False Sharing
 * Validates: Requirements 2.2, 5.3
 */

#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>
#include <vector>

namespace {

constexpr size_t CACHE_LINE_SIZE = 64;

// Packed counters (false sharing)
struct PackedCounters {
    std::atomic<int64_t> counters[4] = {{0}, {0}, {0}, {0}};
};

// Padded counters (no false sharing)
struct alignas(CACHE_LINE_SIZE) PaddedCounter {
    std::atomic<int64_t> value{0};
};

void increment_packed(PackedCounters& c, int id, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        c.counters[id % 4].fetch_add(1, std::memory_order_relaxed);
    }
}

void increment_padded(PaddedCounter* counters, int id, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        counters[id % 4].value.fetch_add(1, std::memory_order_relaxed);
    }
}

static void BM_FalseSharing_Packed(benchmark::State& state) {
    const int num_threads = static_cast<int>(state.range(0));
    const int64_t iterations = 100000;
    
    for (auto _ : state) {
        PackedCounters counters;
        std::vector<std::thread> threads;
        
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back(increment_packed, std::ref(counters), t, iterations);
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        benchmark::DoNotOptimize(counters);
    }
    
    state.SetItemsProcessed(state.iterations() * num_threads * iterations);
}

static void BM_FalseSharing_Padded(benchmark::State& state) {
    const int num_threads = static_cast<int>(state.range(0));
    const int64_t iterations = 100000;
    
    for (auto _ : state) {
        std::vector<PaddedCounter> counters(4);
        std::vector<std::thread> threads;
        
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back(increment_padded, counters.data(), t, iterations);
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        benchmark::DoNotOptimize(counters);
    }
    
    state.SetItemsProcessed(state.iterations() * num_threads * iterations);
}

BENCHMARK(BM_FalseSharing_Packed)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(8)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_FalseSharing_Padded)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(8)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

} // namespace

BENCHMARK_MAIN();
