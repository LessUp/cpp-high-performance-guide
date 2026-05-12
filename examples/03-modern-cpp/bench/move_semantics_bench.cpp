/**
 * @file move_semantics_bench.cpp
 * @brief Benchmark for move semantics
 *
 * Property 6: Move Semantics Performance Advantage
 * Validates: Requirements 3.2
 */

#include <benchmark/benchmark.h>

#include <vector>

#include "buffer.hpp"

namespace {

using hpc::move_semantics::Buffer;

static void BM_Copy_Construction(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    Buffer source(size);

    for (auto _ : state) {
        Buffer copy(source);
        benchmark::DoNotOptimize(copy);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(size));
}

static void BM_Move_Construction(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));

    for (auto _ : state) {
        state.PauseTiming();
        Buffer source(size);
        state.ResumeTiming();

        Buffer moved(std::move(source));
        benchmark::DoNotOptimize(moved);
    }
}

static void BM_Vector_PushBack_Copy(benchmark::State& state) {
    const size_t buffer_size = static_cast<size_t>(state.range(0));
    const int count = 100;

    for (auto _ : state) {
        std::vector<Buffer> vec;
        vec.reserve(count);
        for (int i = 0; i < count; ++i) {
            Buffer buf(buffer_size);
            vec.push_back(buf);  // Copy
        }
        benchmark::DoNotOptimize(vec);
    }
}

static void BM_Vector_PushBack_Move(benchmark::State& state) {
    const size_t buffer_size = static_cast<size_t>(state.range(0));
    const int count = 100;

    for (auto _ : state) {
        std::vector<Buffer> vec;
        vec.reserve(count);
        for (int i = 0; i < count; ++i) {
            Buffer buf(buffer_size);
            vec.push_back(std::move(buf));  // Move
        }
        benchmark::DoNotOptimize(vec);
    }
}

static void BM_Vector_EmplaceBack(benchmark::State& state) {
    const size_t buffer_size = static_cast<size_t>(state.range(0));
    const int count = 100;

    for (auto _ : state) {
        std::vector<Buffer> vec;
        vec.reserve(count);
        for (int i = 0; i < count; ++i) {
            vec.emplace_back(buffer_size);  // Construct in place
        }
        benchmark::DoNotOptimize(vec);
    }
}

BENCHMARK(BM_Copy_Construction)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Move_Construction)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Vector_PushBack_Copy)
    ->RangeMultiplier(4)
    ->Range(1024, 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Vector_PushBack_Move)
    ->RangeMultiplier(4)
    ->Range(1024, 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Vector_EmplaceBack)
    ->RangeMultiplier(4)
    ->Range(1024, 1024 * 1024)
    ->Unit(benchmark::kMillisecond);

}  // namespace

BENCHMARK_MAIN();
