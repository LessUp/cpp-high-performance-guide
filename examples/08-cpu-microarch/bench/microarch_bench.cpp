/**
 * @file microarch_bench.cpp
 * @brief Controlled benchmarks for the microarchitecture effects:
 *        branch predictability and instruction-level parallelism.
 *
 * Kernels come from src/microarch_kernels.cpp (auto-vectorization disabled
 * for that translation unit, so scalar effects are measured honestly).
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <random>
#include <vector>

#include "../src/microarch_kernels.hpp"

namespace {

constexpr std::size_t kSumElements = 1u << 20;
constexpr int kThreshold = 128;

const std::vector<int>& unsorted_data() {
    static const std::vector<int> data = [] {
        std::vector<int> v(kSumElements);
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, 255);
        for (int& x : v) {
            x = dist(rng);
        }
        return v;
    }();
    return data;
}

const std::vector<int>& sorted_data() {
    static const std::vector<int> data = [] {
        std::vector<int> v = unsorted_data();
        std::sort(v.begin(), v.end());
        return v;
    }();
    return data;
}

}  // namespace

static void BM_SumBranched_Unsorted(benchmark::State& state) {
    const std::vector<int>& data = unsorted_data();
    for (auto _ : state) {
        long long sum = microarch::sum_branched(data.data(), data.size(), kThreshold);
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * kSumElements);
}

static void BM_SumBranched_Sorted(benchmark::State& state) {
    const std::vector<int>& data = sorted_data();
    for (auto _ : state) {
        long long sum = microarch::sum_branched(data.data(), data.size(), kThreshold);
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * kSumElements);
}

static void BM_SumTernary(benchmark::State& state) {
    const std::vector<int>& data = unsorted_data();
    for (auto _ : state) {
        long long sum = microarch::sum_ternary(data.data(), data.size(), kThreshold);
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * kSumElements);
}

static void BM_SumMasked(benchmark::State& state) {
    const std::vector<int>& data = unsorted_data();
    for (auto _ : state) {
        long long sum = microarch::sum_masked(data.data(), data.size(), kThreshold);
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * kSumElements);
}

BENCHMARK(BM_SumBranched_Unsorted);
BENCHMARK(BM_SumBranched_Sorted);
BENCHMARK(BM_SumTernary);
BENCHMARK(BM_SumMasked);

// ILP kernels: one benchmark iteration runs `range` chain steps.
static void BM_DepAddChain(benchmark::State& state) {
    const std::size_t iters = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        double r = microarch::dep_add_chain(1.0, iters);
        benchmark::DoNotOptimize(r);
    }
    state.SetItemsProcessed(state.iterations() * iters);
}

static void BM_Ilp4Add(benchmark::State& state) {
    const std::size_t iters = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        double r = microarch::ilp4_add(1.0, iters);
        benchmark::DoNotOptimize(r);
    }
    state.SetItemsProcessed(state.iterations() * iters * 4);
}

static void BM_DepMulChain(benchmark::State& state) {
    const std::size_t iters = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        double r = microarch::dep_mul_chain(1.0, iters);
        benchmark::DoNotOptimize(r);
    }
    state.SetItemsProcessed(state.iterations() * iters);
}

static void BM_Ilp4Mul(benchmark::State& state) {
    const std::size_t iters = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        double r = microarch::ilp4_mul(1.0, iters);
        benchmark::DoNotOptimize(r);
    }
    state.SetItemsProcessed(state.iterations() * iters * 4);
}

BENCHMARK(BM_DepAddChain)->RangeMultiplier(16)->Range(1 << 16, 1 << 24);
BENCHMARK(BM_Ilp4Add)->RangeMultiplier(16)->Range(1 << 16, 1 << 24);
BENCHMARK(BM_DepMulChain)->RangeMultiplier(16)->Range(1 << 16, 1 << 24);
BENCHMARK(BM_Ilp4Mul)->RangeMultiplier(16)->Range(1 << 16, 1 << 24);
