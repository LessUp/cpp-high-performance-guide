#include "simd_utils.hpp"

#if defined(__GNUC__) || defined(__clang__)
#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif
#endif

namespace {

using AddArraysFn = void (*)(const float* a, const float* b, float* c, size_t n);

void add_arrays_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))

__attribute__((target("sse2"))) void add_arrays_sse2(const float* a, const float* b, float* c,
                                                     size_t n) {
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        const __m128 va = _mm_loadu_ps(&a[i]);
        const __m128 vb = _mm_loadu_ps(&b[i]);
        const __m128 vc = _mm_add_ps(va, vb);
        _mm_storeu_ps(&c[i], vc);
    }

    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

__attribute__((target("avx2"))) void add_arrays_avx2(const float* a, const float* b, float* c,
                                                     size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        const __m256 va = _mm256_loadu_ps(&a[i]);
        const __m256 vb = _mm256_loadu_ps(&b[i]);
        const __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }

    add_arrays_sse2(a + i, b + i, c + i, n - i);
}

AddArraysFn resolve_add_arrays() {
    __builtin_cpu_init();
    if (__builtin_cpu_supports("avx2")) {
        return &add_arrays_avx2;
    }
    if (__builtin_cpu_supports("sse2")) {
        return &add_arrays_sse2;
    }
    return &add_arrays_scalar;
}

#else

AddArraysFn resolve_add_arrays() {
    return &add_arrays_scalar;
}

#endif

}  // namespace

namespace hpc::simd {

void dispatch_add_arrays(const float* a, const float* b, float* c, size_t n) {
    static const AddArraysFn dispatch = resolve_add_arrays();
    dispatch(a, b, c, n);
}

}  // namespace hpc::simd
