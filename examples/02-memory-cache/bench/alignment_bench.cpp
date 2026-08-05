/**
 * @file alignment_bench.cpp
 * @brief Benchmark for memory alignment
 *
 * Property 5: Aligned Memory SIMD Performance
 * Validates: Requirements 2.3
 */

#include <benchmark/benchmark.h>

#include <cstdlib>
#include <hpc/memory_utils.hpp>
#include <vector>

#include "scalar_baseline.hpp"

#ifdef __AVX2__
#include <immintrin.h>
#endif

namespace {

constexpr size_t ALIGNMENT = 32;

// Aligned allocation comes from the canonical library (hpc::memory).

// add_scalar lives in scalar_baseline.cpp, compiled with the auto-vectorizer
// disabled — otherwise -O3 -march=native would turn the "scalar" baseline
// into vectorized code and flatten the comparison against AVX.

#ifdef __AVX2__
void add_avx_aligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_load_ps(a + i);
        __m256 vb = _mm256_load_ps(b + i);
        _mm256_store_ps(c + i, _mm256_add_ps(va, vb));
    }
    for (; i < n; ++i)
        c[i] = a[i] + b[i];
}

void add_avx_unaligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        _mm256_storeu_ps(c + i, _mm256_add_ps(va, vb));
    }
    for (; i < n; ++i)
        c[i] = a[i] + b[i];
}
#endif

static void BM_Scalar(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<float> a(n, 1.0f), b(n, 2.0f), c(n);

    for (auto _ : state) {
        scalar_baseline::add_arrays(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(state.iterations() * n * 3 * sizeof(float));
}

#ifdef __AVX2__
static void BM_AVX_Aligned(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    float* a = static_cast<float*>(hpc::memory::aligned_alloc(n * sizeof(float), ALIGNMENT));
    float* b = static_cast<float*>(hpc::memory::aligned_alloc(n * sizeof(float), ALIGNMENT));
    float* c = static_cast<float*>(hpc::memory::aligned_alloc(n * sizeof(float), ALIGNMENT));

    for (size_t i = 0; i < n; ++i) {
        a[i] = 1.0f;
        b[i] = 2.0f;
    }

    for (auto _ : state) {
        add_avx_aligned(a, b, c, n);
        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(state.iterations() * n * 3 * sizeof(float));

    hpc::memory::aligned_free(a);
    hpc::memory::aligned_free(b);
    hpc::memory::aligned_free(c);
}

static void BM_AVX_Unaligned(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<float> buf_a(n + 1, 1.0f), buf_b(n + 1, 2.0f), buf_c(n + 1);
    float* a = buf_a.data() + 1;  // Misalign
    float* b = buf_b.data() + 1;
    float* c = buf_c.data() + 1;

    for (auto _ : state) {
        add_avx_unaligned(a, b, c, n);
        benchmark::DoNotOptimize(c);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(state.iterations() * n * 3 * sizeof(float));
}
#endif

BENCHMARK(BM_Scalar)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

#ifdef __AVX2__
BENCHMARK(BM_AVX_Aligned)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_AVX_Unaligned)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);
#endif

}  // namespace
