/**
 * @file prefetch_bench.cpp
 * @brief Benchmark for software prefetching
 * 
 * Validates: Requirements 2.4
 */

#include <benchmark/benchmark.h>
#include <random>
#include <vector>

namespace {

template<typename T>
inline void prefetch_read(const T* ptr) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ptr, 0, 3);
#endif
}

int64_t sum_no_prefetch(const int64_t* data, size_t n) {
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += data[i];
    }
    return sum;
}

int64_t sum_with_prefetch(const int64_t* data, size_t n) {
    constexpr size_t PREFETCH_DISTANCE = 16;
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        if (i + PREFETCH_DISTANCE < n) {
            prefetch_read(&data[i + PREFETCH_DISTANCE]);
        }
        sum += data[i];
    }
    return sum;
}

int64_t sum_random_no_prefetch(const int64_t* data, const size_t* indices, size_t n) {
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += data[indices[i]];
    }
    return sum;
}

int64_t sum_random_with_prefetch(const int64_t* data, const size_t* indices, size_t n) {
    constexpr size_t PREFETCH_DISTANCE = 8;
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        if (i + PREFETCH_DISTANCE < n) {
            prefetch_read(&data[indices[i + PREFETCH_DISTANCE]]);
        }
        sum += data[indices[i]];
    }
    return sum;
}

static void BM_Sequential_NoPrefetch(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int64_t> data(n);
    for (size_t i = 0; i < n; ++i) data[i] = static_cast<int64_t>(i);
    
    for (auto _ : state) {
        auto result = sum_no_prefetch(data.data(), n);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(int64_t));
}

static void BM_Sequential_WithPrefetch(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int64_t> data(n);
    for (size_t i = 0; i < n; ++i) data[i] = static_cast<int64_t>(i);
    
    for (auto _ : state) {
        auto result = sum_with_prefetch(data.data(), n);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(int64_t));
}

static void BM_Random_NoPrefetch(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int64_t> data(n);
    std::vector<size_t> indices(n);
    
    for (size_t i = 0; i < n; ++i) {
        data[i] = static_cast<int64_t>(i);
        indices[i] = i;
    }
    
    std::mt19937 rng(42);
    for (size_t i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        std::swap(indices[i], indices[dist(rng)]);
    }
    
    for (auto _ : state) {
        auto result = sum_random_no_prefetch(data.data(), indices.data(), n);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(int64_t));
}

static void BM_Random_WithPrefetch(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int64_t> data(n);
    std::vector<size_t> indices(n);
    
    for (size_t i = 0; i < n; ++i) {
        data[i] = static_cast<int64_t>(i);
        indices[i] = i;
    }
    
    std::mt19937 rng(42);
    for (size_t i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        std::swap(indices[i], indices[dist(rng)]);
    }
    
    for (auto _ : state) {
        auto result = sum_random_with_prefetch(data.data(), indices.data(), n);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(int64_t));
}

BENCHMARK(BM_Sequential_NoPrefetch)
    ->RangeMultiplier(4)
    ->Range(1024 * 1024, 64 * 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Sequential_WithPrefetch)
    ->RangeMultiplier(4)
    ->Range(1024 * 1024, 64 * 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Random_NoPrefetch)
    ->RangeMultiplier(4)
    ->Range(1024 * 1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Random_WithPrefetch)
    ->RangeMultiplier(4)
    ->Range(1024 * 1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

} // namespace

BENCHMARK_MAIN();
