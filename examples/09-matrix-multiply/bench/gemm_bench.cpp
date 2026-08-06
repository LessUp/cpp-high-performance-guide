/**
 * @file gemm_bench.cpp
 * @brief Controlled GEMM stage comparison. naive is limited to small sizes
 *        (it is O(n^3) with terrible constants); the later stages scale up.
 */

#include <benchmark/benchmark.h>

#include <hpc/gemm.hpp>
#include <map>
#include <random>
#include <utility>
#include <vector>

namespace {

const std::vector<float>& matrix(int n, unsigned seed) {
    // One cached matrix per (n, seed); setup stays out of the timed region.
    static std::map<std::pair<int, unsigned>, std::vector<float>> cache;
    const auto key = std::make_pair(n, seed);
    auto it = cache.find(key);
    if (it == cache.end()) {
        std::vector<float> v(static_cast<std::size_t>(n) * n);
        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (float& x : v) {
            x = dist(rng);
        }
        it = cache.emplace(key, std::move(v)).first;
    }
    return it->second;
}

}  // namespace

static void BM_GemmNaive(benchmark::State& state) {
    const int n = static_cast<int>(state.range(0));
    const std::vector<float>& a = matrix(n, 1);
    const std::vector<float>& b = matrix(n, 2);
    std::vector<float> c(static_cast<std::size_t>(n) * n);

    for (auto _ : state) {
        hpc::gemm::naive(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
    }
    state.SetBytesProcessed(state.iterations() * 3 * n * n * sizeof(float));
    state.SetItemsProcessed(state.iterations() * 2 * n * n * n);
}

static void BM_GemmTiled(benchmark::State& state) {
    const int n = static_cast<int>(state.range(0));
    const std::vector<float>& a = matrix(n, 1);
    const std::vector<float>& b = matrix(n, 2);
    std::vector<float> c(static_cast<std::size_t>(n) * n);

    for (auto _ : state) {
        hpc::gemm::tiled(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
    }
    state.SetBytesProcessed(state.iterations() * 3 * n * n * sizeof(float));
    state.SetItemsProcessed(state.iterations() * 2 * n * n * n);
}

static void BM_GemmSimd(benchmark::State& state) {
    const int n = static_cast<int>(state.range(0));
    const std::vector<float>& a = matrix(n, 1);
    const std::vector<float>& b = matrix(n, 2);
    std::vector<float> c(static_cast<std::size_t>(n) * n);

    for (auto _ : state) {
        hpc::gemm::simd(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
    }
    state.SetBytesProcessed(state.iterations() * 3 * n * n * sizeof(float));
    state.SetItemsProcessed(state.iterations() * 2 * n * n * n);
}

static void BM_GemmParallel(benchmark::State& state) {
    const int n = static_cast<int>(state.range(0));
    const std::vector<float>& a = matrix(n, 1);
    const std::vector<float>& b = matrix(n, 2);
    std::vector<float> c(static_cast<std::size_t>(n) * n);

    for (auto _ : state) {
        hpc::gemm::parallel(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
    }
    state.SetBytesProcessed(state.iterations() * 3 * n * n * sizeof(float));
    state.SetItemsProcessed(state.iterations() * 2 * n * n * n);
}

BENCHMARK(BM_GemmNaive)->Arg(64)->Arg(128)->Arg(256);
BENCHMARK(BM_GemmTiled)->Arg(128)->Arg(256)->Arg(512);
BENCHMARK(BM_GemmSimd)->Arg(128)->Arg(256)->Arg(512);
BENCHMARK(BM_GemmParallel)->Arg(256)->Arg(512)->Arg(1024);
