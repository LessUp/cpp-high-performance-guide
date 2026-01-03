/**
 * @file vector_reserve_bench.cpp
 * @brief Benchmark for vector reserve
 * 
 * Property 7: Vector Reserve Reduces Allocations
 * Validates: Requirements 3.3
 */

#include <benchmark/benchmark.h>
#include <vector>

namespace {

static void BM_Vector_NoReserve(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    
    for (auto _ : state) {
        std::vector<int> vec;
        for (size_t i = 0; i < n; ++i) {
            vec.push_back(static_cast<int>(i));
        }
        benchmark::DoNotOptimize(vec);
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Vector_WithReserve(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    
    for (auto _ : state) {
        std::vector<int> vec;
        vec.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            vec.push_back(static_cast<int>(i));
        }
        benchmark::DoNotOptimize(vec);
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Vector_Resize(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    
    for (auto _ : state) {
        std::vector<int> vec;
        vec.resize(n);
        for (size_t i = 0; i < n; ++i) {
            vec[i] = static_cast<int>(i);
        }
        benchmark::DoNotOptimize(vec);
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

BENCHMARK(BM_Vector_NoReserve)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Vector_WithReserve)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Vector_Resize)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

} // namespace

BENCHMARK_MAIN();
