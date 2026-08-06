#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <vector>

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
#include <arm_neon.h>
#endif

namespace hpc::simd {

#ifdef __SSE2__
#define HPC_SIMD_HAS_SSE2 1
#define HPC_HAS_SSE2 1
#endif

#ifdef __AVX__
#define HPC_SIMD_HAS_AVX 1
#define HPC_HAS_AVX 1
#endif

#ifdef __AVX2__
#define HPC_SIMD_HAS_AVX2 1
#define HPC_HAS_AVX2 1
#endif

#ifdef __AVX512F__
#define HPC_SIMD_HAS_AVX512 1
#define HPC_HAS_AVX512 1
#endif

#if defined(__aarch64__) && (defined(__GNUC__) || defined(__clang__))
#define HPC_SIMD_HAS_NEON 1
#define HPC_HAS_NEON 1
#endif

enum class SIMDLevel { Scalar, SSE2, AVX, AVX2, AVX512, NEON };

/**
 * @brief Detect the highest SIMD level the CPU supports at runtime.
 *
 * Reports CPU capability, which may exceed what this binary was compiled
 * for: FloatVec / FLOAT_VEC_WIDTH are compile-time choices, while
 * add_arrays() dispatches at runtime across target-attribute variants.
 * Use this for display/diagnostics; get_simd_alignment() derives from it
 * and may over-align relative to the compiled width (harmless).
 */
inline SIMDLevel detect_simd_level() {
#if (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    // NEON is part of the mandatory AArch64 baseline, so no runtime probe.
    return SIMDLevel::NEON;
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __builtin_cpu_init();
    if (__builtin_cpu_supports("avx512f")) {
        return SIMDLevel::AVX512;
    }
    if (__builtin_cpu_supports("avx2")) {
        return SIMDLevel::AVX2;
    }
    if (__builtin_cpu_supports("avx")) {
        return SIMDLevel::AVX;
    }
    if (__builtin_cpu_supports("sse2")) {
        return SIMDLevel::SSE2;
    }
    return SIMDLevel::Scalar;
#elif defined(HPC_SIMD_HAS_AVX512)
    return SIMDLevel::AVX512;
#elif defined(HPC_SIMD_HAS_AVX2)
    return SIMDLevel::AVX2;
#elif defined(HPC_SIMD_HAS_AVX)
    return SIMDLevel::AVX;
#elif defined(HPC_SIMD_HAS_SSE2)
    return SIMDLevel::SSE2;
#else
    return SIMDLevel::Scalar;
#endif
}

inline const char* simd_level_name(SIMDLevel level) {
    switch (level) {
        case SIMDLevel::AVX512:
            return "AVX-512";
        case SIMDLevel::AVX2:
            return "AVX2";
        case SIMDLevel::AVX:
            return "AVX";
        case SIMDLevel::SSE2:
            return "SSE2";
        case SIMDLevel::NEON:
            return "NEON";
        case SIMDLevel::Scalar:
        default:
            return "Scalar";
    }
}

inline std::size_t simd_vector_width(SIMDLevel level) {
    switch (level) {
        case SIMDLevel::AVX512:
            return 64;
        case SIMDLevel::AVX2:
        case SIMDLevel::AVX:
            return 32;
        case SIMDLevel::SSE2:
        case SIMDLevel::NEON:
            return 16;
        case SIMDLevel::Scalar:
        default:
            return sizeof(float);
    }
}

inline std::size_t get_simd_alignment() {
    switch (detect_simd_level()) {
        case SIMDLevel::AVX512:
            return 64;
        case SIMDLevel::AVX2:
        case SIMDLevel::AVX:
            return 32;
        case SIMDLevel::SSE2:
        case SIMDLevel::NEON:
            return 16;
        case SIMDLevel::Scalar:
        default:
            return sizeof(void*);
    }
}

inline bool is_aligned(const void* ptr, std::size_t alignment) {
    return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

inline std::size_t align_up(std::size_t size, std::size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

template <typename T>
class AlignedAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U>;
    };

    AlignedAllocator() = default;

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U>&) {}

    T* allocate(size_type n) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }
        if (n == 0) {
            return nullptr;
        }

        void* ptr = nullptr;
        const std::size_t alignment = get_simd_alignment();
        const std::size_t size = n * sizeof(T);
#if defined(_MSC_VER)
        ptr = _aligned_malloc(size, alignment);
#else
        if (posix_memalign(&ptr, alignment, size) != 0) {
            ptr = nullptr;
        }
#endif
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* ptr, size_type) {
        if (ptr == nullptr) {
            return;
        }
#if defined(_MSC_VER)
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }

    template <typename U>
    bool operator==(const AlignedAllocator<U>&) const {
        return true;
    }

    template <typename U>
    bool operator!=(const AlignedAllocator<U>&) const {
        return false;
    }
};

template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T>>;

template <typename T>
using AlignedBuffer = aligned_vector<T>;

template <typename T>
using aligned_allocator [[deprecated("Use AlignedAllocator<T> directly")]] = AlignedAllocator<T>;

template <typename T>
using simd_allocator = AlignedAllocator<T>;

template <typename T>
aligned_vector<T> make_aligned_vector(std::size_t size) {
    return aligned_vector<T>(size);
}

template <typename T>
aligned_vector<T> make_aligned_vector(std::size_t size, const T& value) {
    return aligned_vector<T>(size, value);
}

template <typename T, std::size_t Width>
class SimdVec;

template <typename T, std::size_t Width>
class SimdVecScalar {
public:
    static constexpr std::size_t width = Width;
    using value_type = T;

    T data[Width];

    SimdVecScalar() = default;

    explicit SimdVecScalar(T value) {
        for (std::size_t i = 0; i < Width; ++i) {
            data[i] = value;
        }
    }

    explicit SimdVecScalar(const T* ptr) {
        for (std::size_t i = 0; i < Width; ++i) {
            data[i] = ptr[i];
        }
    }

    void store(T* ptr) const {
        for (std::size_t i = 0; i < Width; ++i) {
            ptr[i] = data[i];
        }
    }

    T operator[](std::size_t i) const { return data[i]; }
    T& operator[](std::size_t i) { return data[i]; }

    SimdVecScalar operator+(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    SimdVecScalar operator-(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    SimdVecScalar operator*(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = data[i] * other.data[i];
        }
        return result;
    }

    SimdVecScalar operator/(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = data[i] / other.data[i];
        }
        return result;
    }

    SimdVecScalar& operator+=(const SimdVecScalar& other) {
        for (std::size_t i = 0; i < Width; ++i) {
            data[i] += other.data[i];
        }
        return *this;
    }

    SimdVecScalar& operator-=(const SimdVecScalar& other) {
        for (std::size_t i = 0; i < Width; ++i) {
            data[i] -= other.data[i];
        }
        return *this;
    }

    SimdVecScalar& operator*=(const SimdVecScalar& other) {
        for (std::size_t i = 0; i < Width; ++i) {
            data[i] *= other.data[i];
        }
        return *this;
    }

    T horizontal_sum() const {
        T sum = data[0];
        for (std::size_t i = 1; i < Width; ++i) {
            sum += data[i];
        }
        return sum;
    }

    static SimdVecScalar fmadd(const SimdVecScalar& a, const SimdVecScalar& b,
                               const SimdVecScalar& c) {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = a.data[i] * b.data[i] + c.data[i];
        }
        return result;
    }

    SimdVecScalar sqrt() const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = std::sqrt(data[i]);
        }
        return result;
    }

    SimdVecScalar min(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = std::min(data[i], other.data[i]);
        }
        return result;
    }

    SimdVecScalar max(const SimdVecScalar& other) const {
        SimdVecScalar result;
        for (std::size_t i = 0; i < Width; ++i) {
            result.data[i] = std::max(data[i], other.data[i]);
        }
        return result;
    }
};

#ifdef HPC_HAS_SSE2

template <>
class SimdVec<float, 4> {
public:
    static constexpr std::size_t width = 4;
    using value_type = float;

    __m128 data;

    SimdVec() : data(_mm_setzero_ps()) {}
    explicit SimdVec(__m128 value) : data(value) {}
    explicit SimdVec(float value) : data(_mm_set1_ps(value)) {}
    explicit SimdVec(const float* ptr) : data(_mm_loadu_ps(ptr)) {}

    static SimdVec load_aligned(const float* ptr) { return SimdVec(_mm_load_ps(ptr)); }

    void store(float* ptr) const { _mm_storeu_ps(ptr, data); }
    void store_aligned(float* ptr) const { _mm_store_ps(ptr, data); }

    float operator[](std::size_t i) const {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, data);
        return tmp[i];
    }

    SimdVec operator+(const SimdVec& other) const { return SimdVec(_mm_add_ps(data, other.data)); }
    SimdVec operator-(const SimdVec& other) const { return SimdVec(_mm_sub_ps(data, other.data)); }
    SimdVec operator*(const SimdVec& other) const { return SimdVec(_mm_mul_ps(data, other.data)); }
    SimdVec operator/(const SimdVec& other) const { return SimdVec(_mm_div_ps(data, other.data)); }

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

    SimdVec sqrt() const { return SimdVec(_mm_sqrt_ps(data)); }
    SimdVec min(const SimdVec& other) const { return SimdVec(_mm_min_ps(data, other.data)); }
    SimdVec max(const SimdVec& other) const { return SimdVec(_mm_max_ps(data, other.data)); }
};

#endif

#ifdef HPC_HAS_NEON

template <>
class SimdVec<float, 4> {
public:
    static constexpr std::size_t width = 4;
    using value_type = float;

    float32x4_t data;

    SimdVec() : data(vdupq_n_f32(0.0f)) {}
    explicit SimdVec(float32x4_t value) : data(value) {}
    explicit SimdVec(float value) : data(vdupq_n_f32(value)) {}
    explicit SimdVec(const float* ptr) : data(vld1q_f32(ptr)) {}

    // AArch64 NEON loads have no separate aligned variant; both compile to
    // the same instruction. The alias keeps the wrapper interface uniform.
    static SimdVec load_aligned(const float* ptr) { return SimdVec(vld1q_f32(ptr)); }

    void store(float* ptr) const { vst1q_f32(ptr, data); }
    void store_aligned(float* ptr) const { vst1q_f32(ptr, data); }

    float operator[](std::size_t i) const {
        alignas(16) float tmp[4];
        vst1q_f32(tmp, data);
        return tmp[i];
    }

    SimdVec operator+(const SimdVec& other) const { return SimdVec(vaddq_f32(data, other.data)); }
    SimdVec operator-(const SimdVec& other) const { return SimdVec(vsubq_f32(data, other.data)); }
    SimdVec operator*(const SimdVec& other) const { return SimdVec(vmulq_f32(data, other.data)); }
    SimdVec operator/(const SimdVec& other) const { return SimdVec(vdivq_f32(data, other.data)); }

    SimdVec& operator+=(const SimdVec& other) {
        data = vaddq_f32(data, other.data);
        return *this;
    }

    SimdVec& operator-=(const SimdVec& other) {
        data = vsubq_f32(data, other.data);
        return *this;
    }

    SimdVec& operator*=(const SimdVec& other) {
        data = vmulq_f32(data, other.data);
        return *this;
    }

    float horizontal_sum() const { return vaddvq_f32(data); }

    static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c) {
        return SimdVec(vfmaq_f32(c.data, a.data, b.data));
    }

    SimdVec sqrt() const { return SimdVec(vsqrtq_f32(data)); }
    SimdVec min(const SimdVec& other) const { return SimdVec(vminq_f32(data, other.data)); }
    SimdVec max(const SimdVec& other) const { return SimdVec(vmaxq_f32(data, other.data)); }
};

#endif

#ifdef HPC_HAS_AVX2

template <>
class SimdVec<float, 8> {
public:
    static constexpr std::size_t width = 8;
    using value_type = float;

    __m256 data;

    SimdVec() : data(_mm256_setzero_ps()) {}
    explicit SimdVec(__m256 value) : data(value) {}
    explicit SimdVec(float value) : data(_mm256_set1_ps(value)) {}
    explicit SimdVec(const float* ptr) : data(_mm256_loadu_ps(ptr)) {}

    static SimdVec load_aligned(const float* ptr) { return SimdVec(_mm256_load_ps(ptr)); }

    void store(float* ptr) const { _mm256_storeu_ps(ptr, data); }
    void store_aligned(float* ptr) const { _mm256_store_ps(ptr, data); }

    float operator[](std::size_t i) const {
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

    SimdVec sqrt() const { return SimdVec(_mm256_sqrt_ps(data)); }
    SimdVec min(const SimdVec& other) const { return SimdVec(_mm256_min_ps(data, other.data)); }
    SimdVec max(const SimdVec& other) const { return SimdVec(_mm256_max_ps(data, other.data)); }
};

#endif

#ifdef HPC_HAS_AVX512

template <>
class SimdVec<float, 16> {
public:
    static constexpr std::size_t width = 16;
    using value_type = float;

    __m512 data;

    SimdVec() : data(_mm512_setzero_ps()) {}
    explicit SimdVec(__m512 value) : data(value) {}
    explicit SimdVec(float value) : data(_mm512_set1_ps(value)) {}
    explicit SimdVec(const float* ptr) : data(_mm512_loadu_ps(ptr)) {}

    static SimdVec load_aligned(const float* ptr) { return SimdVec(_mm512_load_ps(ptr)); }

    void store(float* ptr) const { _mm512_storeu_ps(ptr, data); }
    void store_aligned(float* ptr) const { _mm512_store_ps(ptr, data); }

    float operator[](std::size_t i) const {
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

    float horizontal_sum() const { return _mm512_reduce_add_ps(data); }

    static SimdVec fmadd(const SimdVec& a, const SimdVec& b, const SimdVec& c) {
        return SimdVec(_mm512_fmadd_ps(a.data, b.data, c.data));
    }

    SimdVec sqrt() const { return SimdVec(_mm512_sqrt_ps(data)); }
    SimdVec min(const SimdVec& other) const { return SimdVec(_mm512_min_ps(data, other.data)); }
    SimdVec max(const SimdVec& other) const { return SimdVec(_mm512_max_ps(data, other.data)); }
};

#endif

#ifdef HPC_HAS_AVX512
using FloatVec = SimdVec<float, 16>;
constexpr std::size_t FLOAT_VEC_WIDTH = 16;
#elif defined(HPC_HAS_AVX2)
using FloatVec = SimdVec<float, 8>;
constexpr std::size_t FLOAT_VEC_WIDTH = 8;
#elif defined(HPC_HAS_SSE2)
using FloatVec = SimdVec<float, 4>;
constexpr std::size_t FLOAT_VEC_WIDTH = 4;
#elif defined(HPC_HAS_NEON)
using FloatVec = SimdVec<float, 4>;
constexpr std::size_t FLOAT_VEC_WIDTH = 4;
#else
using FloatVec = SimdVecScalar<float, 4>;
constexpr std::size_t FLOAT_VEC_WIDTH = 4;
#endif

namespace detail {

using AddArraysFn = void (*)(const float* a, const float* b, float* c, std::size_t n);

inline void add_arrays_scalar(const float* a, const float* b, float* c, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))

__attribute__((target("sse2"))) inline void add_arrays_sse2(const float* a, const float* b,
                                                            float* c, std::size_t n) {
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        const __m128 va = _mm_loadu_ps(a + i);
        const __m128 vb = _mm_loadu_ps(b + i);
        _mm_storeu_ps(c + i, _mm_add_ps(va, vb));
    }
    add_arrays_scalar(a + i, b + i, c + i, n - i);
}

__attribute__((target("avx2,avx"))) inline void add_arrays_avx2(const float* a, const float* b,
                                                                float* c, std::size_t n) {
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);
        _mm256_storeu_ps(c + i, _mm256_add_ps(va, vb));
    }
    add_arrays_sse2(a + i, b + i, c + i, n - i);
}

__attribute__((target("avx512f,avx2,avx"))) inline void add_arrays_avx512(const float* a,
                                                                           const float* b,
                                                                           float* c,
                                                                           std::size_t n) {
    std::size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        const __m512 va = _mm512_loadu_ps(a + i);
        const __m512 vb = _mm512_loadu_ps(b + i);
        _mm512_storeu_ps(c + i, _mm512_add_ps(va, vb));
    }
    add_arrays_avx2(a + i, b + i, c + i, n - i);
}

template <typename Func>
inline Func resolve_best(Func scalar, Func sse2, Func avx2, Func avx512) {
    __builtin_cpu_init();
    if (avx512 && __builtin_cpu_supports("avx512f")) {
        return avx512;
    }
    if (avx2 && __builtin_cpu_supports("avx2")) {
        return avx2;
    }
    if (sse2 && __builtin_cpu_supports("sse2")) {
        return sse2;
    }
    return scalar;
}

#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)

inline void add_arrays_neon(const float* a, const float* b, float* c, std::size_t n) {
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        const float32x4_t va = vld1q_f32(a + i);
        const float32x4_t vb = vld1q_f32(b + i);
        vst1q_f32(c + i, vaddq_f32(va, vb));
    }
    add_arrays_scalar(a + i, b + i, c + i, n - i);
}

#endif

}  // namespace detail

inline void add_arrays(const float* a, const float* b, float* c, std::size_t n) {
    using Fn = detail::AddArraysFn;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    static const Fn dispatch =
        detail::resolve_best<Fn>(&detail::add_arrays_scalar, &detail::add_arrays_sse2,
                                 &detail::add_arrays_avx2, &detail::add_arrays_avx512);
    dispatch(a, b, c, n);
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    detail::add_arrays_neon(a, b, c, n);
#else
    detail::add_arrays_scalar(a, b, c, n);
#endif
}

inline void dispatch_add_arrays(const float* a, const float* b, float* c, std::size_t n) {
    add_arrays(a, b, c, n);
}

inline void multiply_arrays(const float* a, const float* b, float* c, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

inline float dot_product(const float* a, const float* b, std::size_t n) {
    float sum = 0.0f;
    for (std::size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

inline void scale_array(float* arr, float scalar, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

inline void clamp_array(float* arr, float min_val, float max_val, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        arr[i] = std::max(min_val, std::min(max_val, arr[i]));
    }
}

inline void add_arrays_wrapped(const float* a, const float* b, float* c, std::size_t n) {
    std::size_t i = 0;
    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(a + i);
        FloatVec vb(b + i);
        FloatVec vc = va + vb;
        vc.store(c + i);
    }
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

inline float dot_product_wrapped(const float* a, const float* b, std::size_t n) {
    FloatVec sum(0.0f);
    std::size_t i = 0;

    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec va(a + i);
        FloatVec vb(b + i);
        sum = FloatVec::fmadd(va, vb, sum);
    }

    float result = sum.horizontal_sum();
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

inline void scale_array_wrapped(float* arr, float scalar, std::size_t n) {
    FloatVec vscalar(scalar);
    std::size_t i = 0;

    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec value(arr + i);
        value *= vscalar;
        value.store(arr + i);
    }

    for (; i < n; ++i) {
        arr[i] *= scalar;
    }
}

inline void clamp_array_wrapped(float* arr, float min_val, float max_val, std::size_t n) {
    FloatVec vmin(min_val);
    FloatVec vmax(max_val);
    std::size_t i = 0;

    for (; i + FLOAT_VEC_WIDTH <= n; i += FLOAT_VEC_WIDTH) {
        FloatVec value(arr + i);
        value = value.max(vmin).min(vmax);
        value.store(arr + i);
    }

    for (; i < n; ++i) {
        arr[i] = std::max(min_val, std::min(max_val, arr[i]));
    }
}

}  // namespace hpc::simd
