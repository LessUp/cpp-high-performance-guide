/**
 * @file ranges_bench.cpp
 * @brief Benchmark for C++20 Ranges vs Raw Loops
 *
 * Validates: Requirements 3.4
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <numeric>
#include <ranges>
#include <vector>

namespace {

static void BM_Transform_RawLoop(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::vector<int> output(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            output[i] = input[i] * 2 + 1;
        }
        benchmark::DoNotOptimize(output);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Transform_Algorithm(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::vector<int> output(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        std::transform(input.begin(), input.end(), output.begin(), [](int x) { return x * 2 + 1; });
        benchmark::DoNotOptimize(output);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Transform_Ranges(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::vector<int> output(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        std::ranges::transform(input, output.begin(), [](int x) { return x * 2 + 1; });
        benchmark::DoNotOptimize(output);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

// Filter pair: both sides must do the SAME work. The raw loop sums the
// filtered elements (lazy, no materialization), matching the ranges view side
// below. An earlier version materialized the raw-loop output into a vector
// while the view side only summed — measuring "materialize vs sum" instead of
// "raw loop vs ranges", which is misleading. (See the Chain pair for the
// filter+transform variant of the same pattern.)
static void BM_Filter_RawLoop(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        int64_t sum = 0;
        for (int x : input) {
            if (x % 2 == 0) {
                sum += x;
            }
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Filter_RangesView(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        auto view = input | std::views::filter([](int x) { return x % 2 == 0; });
        int64_t sum = 0;
        for (int x : view) {
            sum += x;
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Chain_RawLoop(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        int64_t sum = 0;
        for (int x : input) {
            if (x % 2 == 0) {
                sum += x * 2 + 1;
            }
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Chain_RangesView(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<int> input(n);
    std::iota(input.begin(), input.end(), 0);

    for (auto _ : state) {
        auto view = input | std::views::filter([](int x) { return x % 2 == 0; }) |
                    std::views::transform([](int x) { return x * 2 + 1; });
        int64_t sum = 0;
        for (int x : view) {
            sum += x;
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

BENCHMARK(BM_Transform_RawLoop)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Transform_Algorithm)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Transform_Ranges)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Filter_RawLoop)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Filter_RangesView)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Chain_RawLoop)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Chain_RangesView)
    ->RangeMultiplier(4)
    ->Range(1024, 16 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

}  // namespace
