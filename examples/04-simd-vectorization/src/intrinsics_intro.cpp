/**
 * @file intrinsics_intro.cpp
 * @brief Introduction to SIMD intrinsics (SSE, AVX2, AVX-512)
 * 
 * This example demonstrates:
 * 1. Basic SIMD intrinsics usage
 * 2. SSE (128-bit), AVX2 (256-bit), AVX-512 (512-bit) implementations
 * 3. Performance comparison between scalar and SIMD versions
 */

#include "simd_utils.hpp"
#include <iostream>
#include <cmath>
#include <chrono>

// Include SIMD headers based on availability
#ifdef HPC_HAS_SSE2
    #include <emmintrin.h>  // SSE2
#endif

#ifdef HPC_HAS_SSE4
    #include <smmintrin.h>  // SSE4.1
#endif

#ifdef HPC_HAS_AVX
    #include <immintrin.h>  // AVX, AVX2, AVX-512
#endif

namespace hpc::simd {

// ============================================================================
// Scalar Implementation (Reference)
// ============================================================================

void add_arrays_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void multiply_arrays_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

float dot_product_scalar(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// ============================================================================
// SSE Implementation (128-bit = 4 floats)
// ============================================================================

#ifdef HPC_HAS_SSE2

void add_arrays_sse(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    // Process 4 floats at a time
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);  // Load 4 floats from a
        __m128 vb = _mm_loadu_ps(&b[i]);  // Load 4 floats from b
        __m128 vc = _mm_add_ps(va, vb);   // Add them
        _mm_storeu_ps(&c[i], vc);         // Store result
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void multiply_arrays_sse(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 vc = _mm_mul_ps(va, vb);
        _mm_storeu_ps(&c[i], vc);
    }
    
    for (; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

float dot_product_sse(const float* a, const float* b, size_t n) {
    __m128 sum = _mm_setzero_ps();
    size_t i = 0;
    
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(va, vb);
        sum = _mm_add_ps(sum, prod);
    }
    
    // Horizontal sum of the 4 floats in sum
    // sum = [s0, s1, s2, s3]
    __m128 shuf = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(2, 3, 0, 1));  // [s1, s0, s3, s2]
    sum = _mm_add_ps(sum, shuf);  // [s0+s1, s1+s0, s2+s3, s3+s2]
    shuf = _mm_movehl_ps(shuf, sum);  // [s2+s3, s3+s2, ...]
    sum = _mm_add_ss(sum, shuf);  // [s0+s1+s2+s3, ...]
    
    float result = _mm_cvtss_f32(sum);
    
    // Handle remaining elements
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

#endif // HPC_HAS_SSE2

// ============================================================================
// AVX2 Implementation (256-bit = 8 floats)
// ============================================================================

#ifdef HPC_HAS_AVX2

void add_arrays_avx2(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    // Process 8 floats at a time
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }
    
    // Handle remaining with SSE or scalar
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void multiply_arrays_avx2(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }
    
    for (; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

float dot_product_avx2(const float* a, const float* b, size_t n) {
    __m256 sum = _mm256_setzero_ps();
    size_t i = 0;
    
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        sum = _mm256_fmadd_ps(va, vb, sum);  // FMA: sum += va * vb
    }
    
    // Horizontal sum
    __m128 hi = _mm256_extractf128_ps(sum, 1);
    __m128 lo = _mm256_castps256_ps128(sum);
    __m128 sum128 = _mm_add_ps(hi, lo);
    
    __m128 shuf = _mm_shuffle_ps(sum128, sum128, _MM_SHUFFLE(2, 3, 0, 1));
    sum128 = _mm_add_ps(sum128, shuf);
    shuf = _mm_movehl_ps(shuf, sum128);
    sum128 = _mm_add_ss(sum128, shuf);
    
    float result = _mm_cvtss_f32(sum128);
    
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

#endif // HPC_HAS_AVX2

// ============================================================================
// AVX-512 Implementation (512-bit = 16 floats)
// ============================================================================

#ifdef HPC_HAS_AVX512

void add_arrays_avx512(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    // Process 16 floats at a time
    for (; i + 16 <= n; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __m512 vc = _mm512_add_ps(va, vb);
        _mm512_storeu_ps(&c[i], vc);
    }
    
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void multiply_arrays_avx512(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    for (; i + 16 <= n; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __m512 vc = _mm512_mul_ps(va, vb);
        _mm512_storeu_ps(&c[i], vc);
    }
    
    for (; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

float dot_product_avx512(const float* a, const float* b, size_t n) {
    __m512 sum = _mm512_setzero_ps();
    size_t i = 0;
    
    for (; i + 16 <= n; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        sum = _mm512_fmadd_ps(va, vb, sum);
    }
    
    // Horizontal sum using reduce
    float result = _mm512_reduce_add_ps(sum);
    
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

#endif // HPC_HAS_AVX512

// ============================================================================
// Unified interface with runtime dispatch
// ============================================================================

void add_arrays(const float* a, const float* b, float* c, size_t n) {
#ifdef HPC_HAS_AVX512
    add_arrays_avx512(a, b, c, n);
#elif defined(HPC_HAS_AVX2)
    add_arrays_avx2(a, b, c, n);
#elif defined(HPC_HAS_SSE2)
    add_arrays_sse(a, b, c, n);
#else
    add_arrays_scalar(a, b, c, n);
#endif
}

void multiply_arrays(const float* a, const float* b, float* c, size_t n) {
#ifdef HPC_HAS_AVX512
    multiply_arrays_avx512(a, b, c, n);
#elif defined(HPC_HAS_AVX2)
    multiply_arrays_avx2(a, b, c, n);
#elif defined(HPC_HAS_SSE2)
    multiply_arrays_sse(a, b, c, n);
#else
    multiply_arrays_scalar(a, b, c, n);
#endif
}

float dot_product(const float* a, const float* b, size_t n) {
#ifdef HPC_HAS_AVX512
    return dot_product_avx512(a, b, n);
#elif defined(HPC_HAS_AVX2)
    return dot_product_avx2(a, b, n);
#elif defined(HPC_HAS_SSE2)
    return dot_product_sse(a, b, n);
#else
    return dot_product_scalar(a, b, n);
#endif
}

// ============================================================================
// Demo
// ============================================================================

void demonstrate_intrinsics() {
    constexpr size_t N = 1024 * 1024;
    
    std::cout << "=== SIMD Intrinsics Demo ===" << std::endl;
    std::cout << "Detected SIMD level: " << simd_level_name(detect_simd_level()) << std::endl;
    std::cout << "Array size: " << N << " floats" << std::endl;
    std::cout << std::endl;
    
    // Allocate aligned memory
    AlignedBuffer<float> a(N), b(N), c(N);
    
    // Initialize
    for (size_t i = 0; i < N; ++i) {
        a[i] = static_cast<float>(i) * 0.001f;
        b[i] = static_cast<float>(N - i) * 0.001f;
    }
    
    // Benchmark scalar vs SIMD
    auto benchmark = [&](const char* name, auto func) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int iter = 0; iter < 100; ++iter) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << name << ": " << duration.count() / 100.0 << " us/iteration" << std::endl;
    };
    
    std::cout << "--- Array Addition ---" << std::endl;
    benchmark("Scalar", [&]() { add_arrays_scalar(a.data(), b.data(), c.data(), N); });
#ifdef HPC_HAS_SSE2
    benchmark("SSE   ", [&]() { add_arrays_sse(a.data(), b.data(), c.data(), N); });
#endif
#ifdef HPC_HAS_AVX2
    benchmark("AVX2  ", [&]() { add_arrays_avx2(a.data(), b.data(), c.data(), N); });
#endif
#ifdef HPC_HAS_AVX512
    benchmark("AVX512", [&]() { add_arrays_avx512(a.data(), b.data(), c.data(), N); });
#endif
    
    std::cout << std::endl << "--- Dot Product ---" << std::endl;
    benchmark("Scalar", [&]() { dot_product_scalar(a.data(), b.data(), N); });
#ifdef HPC_HAS_SSE2
    benchmark("SSE   ", [&]() { dot_product_sse(a.data(), b.data(), N); });
#endif
#ifdef HPC_HAS_AVX2
    benchmark("AVX2  ", [&]() { dot_product_avx2(a.data(), b.data(), N); });
#endif
#ifdef HPC_HAS_AVX512
    benchmark("AVX512", [&]() { dot_product_avx512(a.data(), b.data(), N); });
#endif
    
    // Verify correctness
    float scalar_result = dot_product_scalar(a.data(), b.data(), N);
    float simd_result = dot_product(a.data(), b.data(), N);
    std::cout << std::endl << "Correctness check:" << std::endl;
    std::cout << "Scalar dot product: " << scalar_result << std::endl;
    std::cout << "SIMD dot product:   " << simd_result << std::endl;
    std::cout << "Difference: " << std::fabs(scalar_result - simd_result) << std::endl;
}

} // namespace hpc::simd

#ifndef HPC_BENCHMARK_MODE
int main() {
    hpc::simd::demonstrate_intrinsics();
    return 0;
}
#endif
