#pragma once

/**
 * @file simd_wrapper.hpp
 * @brief C++ wrapper for SIMD intrinsics providing a clean, portable interface
 * 
 * This wrapper provides:
 * 1. Type-safe SIMD vector types
 * 2. Operator overloading for natural syntax
 * 3. Automatic fallback to scalar operations
 * 4. Compile-time SIMD level selection
 */

#include "simd_utils.hpp"
#include <cmath>
#include <algorithm>

#ifdef HPC_HAS_SSE2
    #include <emmintrin.h>
#endif

#ifdef HPC_HAS_AVX
    #include <immintrin.h>
#endif

namespace hpc::simd {

// ============================================================================
// Forward declarations
// ============================================================================

template<typename T, size_t Width>
class SimdVec;

// ============================================================================
// Scalar fallback implementation
// ============================================================================

template<typename T, size_t Width>
class SimdVecScalar {
public:
    static constexpr size_t width = Width;
    using value_type = T;
    
    T data[Width];
    
    SimdVecScalar() = default;
    
    explicit SimdVecScalar(T val) {
        for (size_t i = 0; i < Width; ++i) data[i] = val;
    }
    
    SimdVecScalar(const T* ptr) {
        for (size_t i = 0; i < Width; ++i) data[i] = ptr[i];
    }
    
    void store(T* ptr) const {
        for (size_t i = 0; i < Width; ++i) ptr[i] = data[i];
    }
    
    T operator[](size_t i) const { return data[i]; }
    T& operator[](size_t i) { return data[i]; }
    
    SimdVecScalar operator+(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = data[i] + other.data[i];
        return result;
    }
    
    SimdVecScalar operator-(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = data[i] - other.data[i];
        return result;
    }
    
    SimdVecScalar operator*(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = data[i] * other.data[i];
        return result;
    }
    
    SimdVecScalar operator/(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = data[i] / other.data[i];
        return result;
    }
    
    SimdVecScalar& operator+=(const SimdVecScalar& other) {
        for (size_t i = 0; i < Width; ++i) data[i] += other.data[i];
        return *this;
    }
    
    SimdVecScalar& operator-=(const SimdVecScalar& other) {
        for (size_t i = 0; i < Width; ++i) data[i] -= other.data[i];
        return *this;
    }
    
    SimdVecScalar& operator*=(const SimdVecScalar& other) {
        for (size_t i = 0; i < Width; ++i) data[i] *= other.data[i];
        return *this;
    }
    
    T horizontal_sum() const {
        T sum = data[0];
        for (size_t i = 1; i < Width; ++i) sum += data[i];
        return sum;
    }
    
    static SimdVecScalar fmadd(const SimdVecScalar& a, const SimdVecScalar& b, const SimdVecScalar& c) {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = a.data[i] * b.data[i] + c.data[i];
        return result;
    }
    
    SimdVecScalar sqrt() const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = std::sqrt(data[i]);
        return result;
    }
    
    SimdVecScalar min(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = std::min(data[i], other.data[i]);
        return result;
    }
    
    SimdVecScalar max(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (size_t i = 0; i < Width; ++i) result.data[i] = std::max(data[i], other.data[i]);
        return result;
    }
};

// ============================================================================
// SSE Implementation (128-bit, 4 floats)
// ============================================================================

#ifdef HPC_HAS_SSE2

template<>
class SimdVec<float, 4> {
public:
    static constexpr size_t width = 4;
    using value_type = float;
    
    __m128 data;
    
    SimdVec() : data(_mm_setzero_ps()) {}
    
    explicit SimdVec(__m128 v) : data(v) {}
    
    explicit SimdVec(float val) : data(_mm_set1_ps(val)) {}
    
    SimdVec(const float* ptr) : data(_mm_loadu_ps(ptr)) {}
    
    static SimdVec load_aligned(const float* ptr) {
        return SimdVec(_mm_load_ps(ptr));
    }
    
    void store(float* ptr) const {
        _mm_storeu_ps(ptr, data);
    }
    
    void store_aligned(float* ptr) const {
        _mm_store_ps(ptr, data);
    }
    
    float operator[](size_t i) const {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, data);
        return tmp[i];
    }
    
    SimdVec operator+(const SimdVec& other) const {
        return SimdVec(_mm_add_ps(data, other.data));
    }
    
    SimdVec operator-(const SimdVec& other) const {
        return SimdVec(_mm_sub_ps(data, other.data));
    }
    
    SimdVec operator*(const SimdVec& other) const {
        return SimdVec(_mm_mul_ps(data, other.data));
    }
    
    SimdVec operator/(const SimdVec& other) const {
        return SimdVec(_mm_div_ps(data, other.data));
    }
    
    SimdVec& operator+=(const SimdVec& other) {
        data = _mm_add_ps(data, other.data);
        return *this;
    }
    
    SimdVec& operator-=(const SimdVec& other) {
        data = _mm_sub_ps(data, other.data);
        return *this;
    }
    
    SimdVec& operator*=(const SimdVec& other) {
        data = _mm_mul_ps(data, other.data);
        return *this;
    }
    
    float horizontal_sum() const {
        __m128 shuf = _mm_shuffle_ps(data, data, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 sums = _mm_add_ps(data, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        return _mm_cvtss_f32(sums);
    }
    
    static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c) {
#ifdef HPC_HAS_AVX2
        return SimdVec(_mm_fmadd_ps(a.data, b.data, c.data));
#else
        return SimdVec(_mm_add_ps(_mm_mul_ps(a.data, b.data), c.data));
#endif
    }
    
    SimdVec sqrt() const {
        return SimdVec(_mm_sqrt_ps(data));
    }
    
    SimdVec min(const SimdVec& other) const {
        return SimdVec(_mm_min_ps(data, other.data));
    }
    
    SimdVec max(const SimdVec& other) const {
        return SimdVec(_mm_max_ps(data, other.data));
    }
};

#endif // HPC_HAS_SSE2

// ============================================================================
// AVX2 Implementation (256-bit, 8 floats)
// ============================================================================

#ifdef HPC_HAS_AVX2

template<>
class SimdVec<float, 8> {
public:
    static constexpr size_t width = 8;
    using value_type = float;
    
    __m256 data;
    
    SimdVec() : data(_mm256_setzero_ps()) {}
    
    explicit SimdVec(__m256 v) : data(v) {}
    
    explicit SimdVec(float val) : data(_mm256_set1_ps(val)) {}
    
    SimdVec(const float* ptr) : data(_mm256_loadu_ps(ptr)) {}
    
    static SimdVec load_aligned(const float* ptr) {
        return SimdVec(_mm256_load_ps(ptr));
    }
    
    void store(float* ptr) const {
        _mm256_storeu_ps(ptr, data);
    }
    
    void store_aligned(float* ptr) const {
        _mm256_store_ps(ptr, data);
    }
    
    float operator[](size_t i) const {
        alignas(32) float tmp[8];
        _mm256_store_ps(tmp, data);
        return tmp[i];
    }
    
    SimdVec operator+(const SimdVec& other) const {
        return SimdVec(_mm256_add_ps(data, other.data));
    }
    
    SimdVec operator-(const SimdVec& other) const {
        return SimdVec(_mm256_sub_ps(data, other.data));
    }
    
    SimdVec operator*(const SimdVec& other) const {
        return SimdVec(_mm256_mul_ps(data, other.data));
    }
    
    SimdVec operator/(const SimdVec& other) const {
        return SimdVec(_mm256_div_ps(data, other.data));
    }
    
    SimdVec& operator+=(const SimdVec& other) {
        data = _mm256_add_ps(data, other.data);
        return *this;
    }
    
    SimdVec& operator-=(const SimdVec& other) {
        data = _mm256_sub_ps(data, other.data);
        return *this;
    }
    
    SimdVec& operator*=(const SimdVec& other) {
        data = _mm256_mul_ps(data, other.data);
        return *this;
    }
    
    float horizontal_sum() const {
        __m128 hi = _mm256_extractf128_ps(data, 1);
        __m128 lo = _mm256_castps256_ps128(data);
        __m128 sum128 = _mm_add_ps(hi, lo);
        __m128 shuf = _mm_shuffle_ps(sum128, sum128, _MM_SHUFFLE(2, 3, 0, 1));
        sum128 = _mm_add_ps(sum128, shuf);
        shuf = _mm_movehl_ps(shuf, sum128);
        sum128 = _mm_add_ss(sum128, shuf);
        return _mm_cvtss_f32(sum128);
    }
    
    static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c) {
        return SimdVec(_mm256_fmadd_ps(a.data, b.data, c.data));
    }
    
    SimdVec sqrt() const {
        return SimdVec(_mm256_sqrt_ps(data));
    }
    
    SimdVec min(const SimdVec& other) const {
        return SimdVec(_mm256_min_ps(data, other.data));
    }
    
    SimdVec max(const SimdVec& other) const {
        return SimdVec(_mm256_max_ps(data, other.data));
    }
};

#endif // HPC_HAS_AVX2

// ============================================================================
// AVX-512 Implementation (512-bit, 16 floats)
// ============================================================================

#ifdef HPC_HAS_AVX512

template<>
class SimdVec<float, 16> {
public:
    static constexpr size_t width = 16;
    using value_type = float;
    
    __m512 data;
    
    SimdVec() : data(_mm512_setzero_ps()) {}
    
    explicit SimdVec(__m512 v) : data(v) {}
    
    explicit SimdVec(float val) : data(_mm512_set1_ps(val)) {}
    
    SimdVec(const float* ptr) : data(_mm512_loadu_ps(ptr)) {}
    
    static SimdVec load_aligned(const float* ptr) {
        return SimdVec(_mm512_load_ps(ptr));
    }
    
    void store(float* ptr) const {
        _mm512_storeu_ps(ptr, data);
    }
    
    void store_aligned(float* ptr) const {
        _mm512_store_ps(ptr, data);
    }
    
    float operator[](size_t i) const {
        alignas(64) float tmp[16];
        _mm512_store_ps(tmp, data);
        return tmp[i];
    }
    
    SimdVec operator+(const SimdVec& other) const {
        return SimdVec(_mm512_add_ps(data, other.data));
    }
    
    SimdVec operator-(const SimdVec& other) const {
        return SimdVec(_mm512_sub_ps(data, other.data));
    }
    
    SimdVec operator*(const SimdVec& other) const {
        return SimdVec(_mm512_mul_ps(data, other.data));
    }
    
    SimdVec operator/(const SimdVec& other) const {
        return SimdVec(_mm512_div_ps(data, other.data));
    }
    
    SimdVec& operator+=(const SimdVec& other) {
        data = _mm512_add_ps(data, other.data);
        return *this;
    }
    
    SimdVec& operator-=(const SimdVec& other) {
        data = _mm512_sub_ps(data, other.data);
        return *this;
    }
    
    SimdVec& operator*=(const SimdVec& other) {
        data = _mm512_mul_ps(data, other.data);
        return *this;
    }
    
    float horizontal_sum() const {
        return _mm512_reduce_add_ps(data);
    }
    
    static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c) {
        return SimdVec(_mm512_fmadd_ps(a.data, b.data, c.data));
    }
    
    SimdVec sqrt() const {
        return SimdVec(_mm512_sqrt_ps(data));
    }
    
    SimdVec min(const SimdVec& other) const {
        return SimdVec(_mm512_min_ps(data, other.data));
    }
    
    SimdVec max(const SimdVec& other) const {
        return SimdVec(_mm512_max_ps(data, other.data));
    }
};

#endif // HPC_HAS_AVX512

// ============================================================================
// Type aliases for convenience
// ============================================================================

#ifdef HPC_HAS_AVX512
    using FloatVec = SimdVec<float, 16>;
    constexpr size_t FLOAT_VEC_WIDTH = 16;
#elif defined(HPC_HAS_AVX2)
    using FloatVec = SimdVec<float, 8>;
    constexpr size_t FLOAT_VEC_WIDTH = 8;
#elif defined(HPC_HAS_SSE2)
    using FloatVec = SimdVec<float, 4>;
    constexpr size_t FLOAT_VEC_WIDTH = 4;
#else
    using FloatVec = SimdVecScalar<float, 4>;
    constexpr size_t FLOAT_VEC_WIDTH = 4;
#endif

// ============================================================================
// High-level operations using the wrapper
// ============================================================================

/// Add two arrays using SIMD wrapper
inline void add_arrays_wrapped(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(&a[i]);
        FloatVec vb(&b[i]);
        FloatVec vc = va + vb;
        vc.store(&c[i]);
    }
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

/// Dot product using SIMD wrapper
inline float dot_product_wrapped(const float* a, const float* b, size_t n) {
    FloatVec sum(0.0f);
    size_t i = 0;
    
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(&a[i]);
        FloatVec vb(&b[i]);
        sum = FloatVec::fmadd(va, vb, sum);
    }
    
    float result = sum.horizontal_sum();
    
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

/// Scale array by scalar using SIMD wrapper
inline void scale_array_wrapped(float* arr, float scalar, size_t n) {
    FloatVec vscalar(scalar);
    size_t i = 0;
    
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec v(&arr[i]);
        v *= vscalar;
        v.store(&arr[i]);
    }
    
    for (; i < n; ++i) {
        arr[i] *= scalar;
    }
}

/// Clamp array values using SIMD wrapper
inline void clamp_array_wrapped(float* arr, float min_val, float max_val, size_t n) {
    FloatVec vmin(min_val);
    FloatVec vmax(max_val);
    size_t i = 0;
    
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec v(&arr[i]);
        v = v.max(vmin).min(vmax);
        v.store(&arr[i]);
    }
    
    for (; i < n; ++i) {
        arr[i] = std::max(min_val, std::min(max_val, arr[i]));
    }
}

} // namespace hpc::simd
