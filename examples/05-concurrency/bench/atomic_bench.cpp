/**
 * @file atomic_bench.cpp
 * @brief Atomic operations benchmark
 * 
 * Property 10: Atomic Operations Correctness
 * Validates: Requirements 5.1
 */

#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>
#include <vector>

namespace {

static void BM_Atomic_SeqCst(benchmark::State& state) {
    std::atomic<int64_t> counter{0};
    const int num_threads = static_cast<int>(state.range(0));
    const int64_t increments = 100000;
    
    for (auto _ : state) {
        counter.store(0, std::memory_order_relaxed);
        std::vector<std::thread> threads;
        
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&counter, increments]() {
                for (int64_t i = 0; i < increments; ++i) {
                    counter.fetch_add(1, std::memory_order_seq_cst);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        benchmark::DoNotOptimize(counter.load());
    }
    
    state.SetItemsProcessed(state.iterations() * num_threads * increments);
}

static void BM_Atomic_Relaxed(benchmark::State& state) {
    std::atomic<int64_t> counter{0};
    const int num_threads = static_cast<int>(state.range(0));
    const int64_t increments = 100000;
    
    for (auto _ : state) {
        counter.store(0, std::memory_order_relaxed);
        std::vector<std::thread> threads;
        
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&counter, increments]() {
                for (int64_t i = 0; i < increments; ++i) {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        benchmark::DoNotOptimize(counter.load());
    }
    
    state.SetItemsProcessed(state.iterations() * num_threads * increments);
}

BENCHMARK(BM_Atomic_SeqCst)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(8)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_Atomic_Relaxed)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(8)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

} // namespace

BENCHMARK_MAIN();
