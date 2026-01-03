/**
 * @file openmp_bench.cpp
 * @brief OpenMP scaling benchmark
 * 
 * Property 12: OpenMP Scaling Efficiency
 * Validates: Requirements 5.4, 5.5
 */

#include <benchmark/benchmark.h>
#include <cmath>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

static void BM_OpenMP_ParallelFor(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    const int num_threads = static_cast<int>(state.range(1));
    std::vector<double> data(n);
    
#ifdef _OPENMP
    omp_set_num_threads(num_threads);
#endif
    
    for (auto _ : state) {
#ifdef _OPENMP
        #pragma omp parallel for
#endif
        for (size_t i = 0; i < n; ++i) {
            data[i] = std::sin(static_cast<double>(i) * 0.001) *
                      std::cos(static_cast<double>(i) * 0.002);
        }
        benchmark::DoNotOptimize(data.data());
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
    state.SetLabel(std::to_string(num_threads) + " threads");
}

static void BM_OpenMP_Reduction(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    const int num_threads = static_cast<int>(state.range(1));
    std::vector<double> data(n);
    
    for (size_t i = 0; i < n; ++i) {
        data[i] = static_cast<double>(i % 1000) * 0.001;
    }
    
#ifdef _OPENMP
    omp_set_num_threads(num_threads);
#endif
    
    for (auto _ : state) {
        double sum = 0.0;
#ifdef _OPENMP
        #pragma omp parallel for reduction(+:sum)
#endif
        for (size_t i = 0; i < n; ++i) {
            sum += data[i];
        }
        benchmark::DoNotOptimize(sum);
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

BENCHMARK(BM_OpenMP_ParallelFor)
    ->Args({10000000, 1})
    ->Args({10000000, 2})
    ->Args({10000000, 4})
    ->Args({10000000, 8})
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_OpenMP_Reduction)
    ->Args({10000000, 1})
    ->Args({10000000, 2})
    ->Args({10000000, 4})
    ->Args({10000000, 8})
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

} // namespace

BENCHMARK_MAIN();
