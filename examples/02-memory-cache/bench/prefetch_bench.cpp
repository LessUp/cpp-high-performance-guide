/**
 * @file prefetch_bench.cpp
 * @brief Benchmark for software prefetching
 *
 * Validates: Requirements 2.4
 *
 * The traversal routines under test come from the canonical library
 * (hpc/memory_utils.hpp) — the same implementations demonstrated by
 * src/prefetch.cpp, so benchmark numbers and example output describe
 * identical code.
 */

#include <benchmark/benchmark.h>

#include <cstdint>
#include <hpc/memory_utils.hpp>
#include <random>
#include <vector>

namespace {

static void BM_Sequential_NoPrefetch(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int64_t> data(n);
    for (size_t i = 0; i < n; ++i)
        data[i] = static_cast<int64_t>(i);

    for (auto _ : state) {
        auto result = hpc::memory::sum_no_prefetch(data.data(), n);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(state.iterations() * n * sizeof(int64_t));
}

static void BM_Sequential_WithPrefetch(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int64_t> data(n);
    for (size_t i = 0; i < n; ++i)
        data[i] = static_cast<int64_t>(i);

    for (auto _ : state) {
        auto result = hpc::memory::sum_with_prefetch(data.data(), n);
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
        auto result = hpc::memory::sum_random_no_prefetch(data.data(), indices.data(), n);
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
        auto result = hpc::memory::sum_random_with_prefetch(data.data(), indices.data(), n);
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

}  // namespace
