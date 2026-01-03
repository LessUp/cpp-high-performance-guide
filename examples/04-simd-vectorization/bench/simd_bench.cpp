/**
 * @file simd_bench.cpp
 * @brief Benchmarks comparing scalar vs SIMD implementations
 * 
 * This benchmark measures:
 * 1. Scalar vs SSE vs AVX2 vs AVX-512 performance
 * 2. Speedup ratios for different operations
 * 3. Impact of array size on SIMD efficiency
 */

#include <benchmark/benchmark.h>
#include "../include/simd_utils.hpp"
#include "../include/simd_wrapper.hpp"
#include <vector>
#include <random>
#include <cmath>

namespace {

// Initialize arrays with random data
void init_random(float* arr, size_t n) {
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    for (size_t i = 0; i < n; ++i) {
        arr[i] = dist(gen);
    }
}

// Scalar implementations for comparison
void add_arrays_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

float dot_product_scalar(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

void scale_array_scalar(float* arr, float scalar, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

} // anonymous namespace

// ============================================================================
// Array Addition Benchmarks
// ============================================================================

static void BM_AddArrays_Scalar(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> a(n), b(n), c(n);
    init_random(a.data(), n);
    init_random(b.data(), n);
    
    for (auto _ : state) {
        add_arrays_scalar(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 3);
    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_AddArrays_SIMD(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> a(n), b(n), c(n);
    init_random(a.data(), n);
    init_random(b.data(), n);
    
    for (auto _ : state) {
        hpc::simd::add_arrays_wrapped(a.data(), b.data(), c.data(), n);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 3);
    state.SetItemsProcessed(state.iterations() * n);
    state.SetLabel(hpc::simd::simd_level_name(hpc::simd::detect_simd_level()));
}

BENCHMARK(BM_AddArrays_Scalar)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_AddArrays_SIMD)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// ============================================================================
// Dot Product Benchmarks
// ============================================================================

static void BM_DotProduct_Scalar(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> a(n), b(n);
    init_random(a.data(), n);
    init_random(b.data(), n);
    
    for (auto _ : state) {
        float result = dot_product_scalar(a.data(), b.data(), n);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_DotProduct_SIMD(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> a(n), b(n);
    init_random(a.data(), n);
    init_random(b.data(), n);
    
    for (auto _ : state) {
        float result = hpc::simd::dot_product_wrapped(a.data(), b.data(), n);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
    state.SetLabel(hpc::simd::simd_level_name(hpc::simd::detect_simd_level()));
}

BENCHMARK(BM_DotProduct_Scalar)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_DotProduct_SIMD)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// ============================================================================
// Scale Array Benchmarks
// ============================================================================

static void BM_ScaleArray_Scalar(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> arr(n);
    init_random(arr.data(), n);
    const float scalar = 2.5f;
    
    for (auto _ : state) {
        state.PauseTiming();
        init_random(arr.data(), n);
        state.ResumeTiming();
        
        scale_array_scalar(arr.data(), scalar, n);
        benchmark::DoNotOptimize(arr.data());
        benchmark::ClobberMemory();
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_ScaleArray_SIMD(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> arr(n);
    init_random(arr.data(), n);
    const float scalar = 2.5f;
    
    for (auto _ : state) {
        state.PauseTiming();
        init_random(arr.data(), n);
        state.ResumeTiming();
        
        hpc::simd::scale_array_wrapped(arr.data(), scalar, n);
        benchmark::DoNotOptimize(arr.data());
        benchmark::ClobberMemory();
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
    state.SetLabel(hpc::simd::simd_level_name(hpc::simd::detect_simd_level()));
}

BENCHMARK(BM_ScaleArray_Scalar)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_ScaleArray_SIMD)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// ============================================================================
// Clamp Array Benchmarks
// ============================================================================

static void BM_ClampArray_Scalar(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> arr(n);
    init_random(arr.data(), n);
    
    for (auto _ : state) {
        state.PauseTiming();
        init_random(arr.data(), n);
        state.ResumeTiming();
        
        for (size_t i = 0; i < n; ++i) {
            arr[i] = std::max(-50.0f, std::min(50.0f, arr[i]));
        }
        benchmark::DoNotOptimize(arr.data());
        benchmark::ClobberMemory();
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
}

static void BM_ClampArray_SIMD(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    hpc::simd::AlignedBuffer<float> arr(n);
    init_random(arr.data(), n);
    
    for (auto _ : state) {
        state.PauseTiming();
        init_random(arr.data(), n);
        state.ResumeTiming();
        
        hpc::simd::clamp_array_wrapped(arr.data(), -50.0f, 50.0f, n);
        benchmark::DoNotOptimize(arr.data());
        benchmark::ClobberMemory();
    }
    
    state.SetBytesProcessed(state.iterations() * n * sizeof(float) * 2);
    state.SetItemsProcessed(state.iterations() * n);
    state.SetLabel(hpc::simd::simd_level_name(hpc::simd::detect_simd_level()));
}

BENCHMARK(BM_ClampArray_Scalar)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_ClampArray_SIMD)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// ============================================================================
// FloatVec Operations Benchmarks
// ============================================================================

static void BM_FloatVec_FMA(benchmark::State& state) {
    constexpr size_t WIDTH = hpc::simd::FLOAT_VEC_WIDTH;
    alignas(64) float a_data[WIDTH], b_data[WIDTH], c_data[WIDTH];
    
    for (size_t i = 0; i < WIDTH; ++i) {
        a_data[i] = static_cast<float>(i);
        b_data[i] = static_cast<float>(i) * 0.5f;
        c_data[i] = 1.0f;
    }
    
    hpc::simd::FloatVec va(a_data);
    hpc::simd::FloatVec vb(b_data);
    hpc::simd::FloatVec vc(c_data);
    
    for (auto _ : state) {
        hpc::simd::FloatVec result = hpc::simd::FloatVec::fmadd(va, vb, vc);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetItemsProcessed(state.iterations() * WIDTH);
}

BENCHMARK(BM_FloatVec_FMA);

static void BM_FloatVec_HorizontalSum(benchmark::State& state) {
    constexpr size_t WIDTH = hpc::simd::FLOAT_VEC_WIDTH;
    alignas(64) float data[WIDTH];
    
    for (size_t i = 0; i < WIDTH; ++i) {
        data[i] = static_cast<float>(i);
    }
    
    hpc::simd::FloatVec v(data);
    
    for (auto _ : state) {
        float result = v.horizontal_sum();
        benchmark::DoNotOptimize(result);
    }
    
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_FloatVec_HorizontalSum);

BENCHMARK_MAIN();
