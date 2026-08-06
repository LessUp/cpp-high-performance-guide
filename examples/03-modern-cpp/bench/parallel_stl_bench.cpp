/**
 * @file parallel_stl_bench.cpp
 * @brief Controlled comparison of plain loop vs std::transform execution
 *        policies. par/par_unseq are included only when TBB was found at
 *        configure time (see src/parallel_stl.cpp for the policy semantics).
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cmath>
#include <execution>
#include <numeric>
#include <vector>

namespace {

float workload(float x) {
    return std::sqrt(x) * 0.5f + 1.0f;
}

void run(benchmark::State& state, auto policy) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<float> in(n), out(n);
    std::iota(in.begin(), in.end(), 0.0f);

    for (auto _ : state) {
        std::transform(policy, in.begin(), in.end(), out.begin(),
                       [](float x) { return workload(x); });
        benchmark::DoNotOptimize(out.data());
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
}

}  // namespace

static void BM_Transform_PlainLoop(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<float> in(n), out(n);
    std::iota(in.begin(), in.end(), 0.0f);

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            out[i] = workload(in[i]);
        }
        benchmark::DoNotOptimize(out.data());
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_Transform_Seq(benchmark::State& state) {
    run(state, std::execution::seq);
}

static void BM_Transform_Unseq(benchmark::State& state) {
    run(state, std::execution::unseq);
}

#ifdef HPC_HAS_TBB
static void BM_Transform_Par(benchmark::State& state) {
    run(state, std::execution::par);
}

static void BM_Transform_ParUnseq(benchmark::State& state) {
    run(state, std::execution::par_unseq);
}
#endif

BENCHMARK(BM_Transform_PlainLoop)->RangeMultiplier(4)->Range(1 << 12, 1 << 22);
BENCHMARK(BM_Transform_Seq)->RangeMultiplier(4)->Range(1 << 12, 1 << 22);
BENCHMARK(BM_Transform_Unseq)->RangeMultiplier(4)->Range(1 << 12, 1 << 22);
#ifdef HPC_HAS_TBB
BENCHMARK(BM_Transform_Par)->RangeMultiplier(4)->Range(1 << 12, 1 << 22);
BENCHMARK(BM_Transform_ParUnseq)->RangeMultiplier(4)->Range(1 << 12, 1 << 22);
#endif
