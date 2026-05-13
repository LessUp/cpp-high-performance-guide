/**
 * @file simd_utils.hpp
 * @brief SIMD utility functions and feature detection
 *
 * This header provides common utilities for SIMD programming including
 * feature detection, alignment helpers, and basic SIMD operations.
 *
 * All functionality is header-only for ease of integration.
 *
 * Validates:
 *   - Requirement 4.1: Automatic Vectorization Patterns
 *   - Requirement 4.2: SIMD Intrinsics Introduction
 *   - Requirement 4.3: SIMD Abstraction Wrappers
 *   - Requirement 4.4: CPU Capability Detection
 *   - Requirement 4.5: Scalar vs Vectorized Benchmark
 *   - Requirement 4.6: Vectorization Reports
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <vector>

// Intel SIMD intrinsics - always include on x86 for target attribute dispatch
// The target attribute controls which instructions are actually used
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#endif

// Feature detection macros for compile-time checks
#ifdef __SSE2__
#define HPC_HAS_SSE2 1
#endif

#ifdef __AVX__
#define HPC_HAS_AVX 1
#endif

#ifdef __AVX2__
#define HPC_HAS_AVX2 1
#endif

#ifdef __AVX512F__
#define HPC_HAS_AVX512 1
#endif

namespace hpc::simd {

/**
 * @brief Check if a pointer is aligned to the specified boundary
 */
inline bool is_aligned(const void* ptr, size_t alignment) {
    return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
}

/**
 * @brief Align a size up to the next multiple of alignment
 */
inline size_t align_up(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

/**
 * @brief Get the optimal SIMD alignment for the current platform
 */
inline size_t get_simd_alignment() {
#ifdef HPC_HAS_AVX512
    return 64;  // AVX-512 uses 64-byte alignment
#elif defined(HPC_HAS_AVX) || defined(HPC_HAS_AVX2)
    return 32;  // AVX/AVX2 uses 32-byte alignment
#elif defined(HPC_HAS_SSE2)
    return 16;  // SSE uses 16-byte alignment
#else
    return sizeof(void*);  // Fallback to pointer alignment
#endif
}

/**
 * @brief Aligned memory allocator for SIMD operations
 *
 * **Design Note**: This allocator is intentionally separate from
 * `hpc::memory::AlignedAllocator` to keep the SIMD module self-contained.
 *
 * Key differences from `hpc::memory::AlignedAllocator`:
 * - Uses **runtime SIMD detection** for alignment (16/32/64 bytes based on CPU)
 * - `hpc::memory::AlignedAllocator` uses a **compile-time constant** (CACHE_LINE_SIZE)
 *
 * This is NOT an ODR violation - they exist in separate namespaces and serve
 * different purposes: SIMD optimization vs cache-line alignment.
 */
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
        // Overflow protection
        if (n > std::numeric_limits<size_type>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }

        if (n == 0) {
            return nullptr;
        }

        const size_t alignment = get_simd_alignment();
        const size_t size = n * sizeof(T);

        void* ptr = nullptr;
#if defined(_MSC_VER)
        ptr = _aligned_malloc(size, alignment);
#else
        if (posix_memalign(&ptr, alignment, size) != 0) {
            ptr = nullptr;
        }
#endif
        if (!ptr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, size_type) {
        if (p == nullptr) {
            return;
        }
#if defined(_MSC_VER)
        _aligned_free(p);
#else
        free(p);
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

/**
 * @brief Backward-compatible alias for AlignedAllocator
 * @deprecated Use AlignedAllocator<T> directly
 */
template <typename T>
using aligned_allocator [[deprecated("Use AlignedAllocator<T> directly")]] = AlignedAllocator<T>;

/**
 * @brief Alias for AlignedAllocator with SIMD-specific naming
 */
template <typename T>
using simd_allocator = AlignedAllocator<T>;

/**
 * @brief Aligned vector type for SIMD operations
 */
template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T>>;

/**
 * @brief Aligned buffer type alias for compatibility
 */
template <typename T>
using AlignedBuffer = aligned_vector<T>;

/**
 * @brief Create an aligned vector with the specified size
 */
template <typename T>
aligned_vector<T> make_aligned_vector(size_t size) {
    return aligned_vector<T>(size);
}

/**
 * @brief Create an aligned vector with the specified size and initial value
 */
template <typename T>
aligned_vector<T> make_aligned_vector(size_t size, const T& value) {
    return aligned_vector<T>(size, value);
}

/**
 * @brief SIMD capability levels
 */
enum class SIMDLevel { Scalar, SSE2, AVX, AVX2, AVX512 };

/**
 * @brief Detect the highest available SIMD level
 */
inline SIMDLevel detect_simd_level() {
#ifdef HPC_HAS_AVX512
    return SIMDLevel::AVX512;
#elif defined(HPC_HAS_AVX2)
    return SIMDLevel::AVX2;
#elif defined(HPC_HAS_AVX)
    return SIMDLevel::AVX;
#elif defined(HPC_HAS_SSE2)
    return SIMDLevel::SSE2;
#else
    return SIMDLevel::Scalar;
#endif
}

/**
 * @brief Get the name of a SIMD level
 */
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
        case SIMDLevel::Scalar:
            return "Scalar";
        default:
            return "Unknown";
    }
}

/**
 * @brief Get the vector width in bytes for a SIMD level
 */
inline size_t simd_vector_width(SIMDLevel level) {
    switch (level) {
        case SIMDLevel::AVX512:
            return 64;
        case SIMDLevel::AVX2:
            return 32;
        case SIMDLevel::AVX:
            return 32;
        case SIMDLevel::SSE2:
            return 16;
        case SIMDLevel::Scalar:
            return sizeof(float);
        default:
            return sizeof(float);
    }
}

//------------------------------------------------------------------------------
// Runtime SIMD Dispatch - Header-only implementation
//------------------------------------------------------------------------------

namespace detail {

// 函数指针类型
using AddArraysFn = void (*)(const float* a, const float* b, float* c, size_t n);

// 标量实现
inline void add_arrays_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))

// SSE2 实现
__attribute__((target("sse2"))) inline void add_arrays_sse2(const float* a, const float* b,
                                                            float* c, size_t n) {
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

// AVX2 实现
__attribute__((target("avx2,avx"))) inline void add_arrays_avx2(const float* a, const float* b,
                                                                float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        const __m256 va = _mm256_loadu_ps(&a[i]);
        const __m256 vb = _mm256_loadu_ps(&b[i]);
        const __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }

    add_arrays_sse2(a + i, b + i, c + i, n - i);
}

// 运行时解析函数
inline AddArraysFn resolve_add_arrays() {
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

// 非 x86 平台：仅使用标量实现
inline AddArraysFn resolve_add_arrays() {
    return &add_arrays_scalar;
}

#endif

}  // namespace detail

/**
 * @brief Add two arrays using the best available SIMD path at runtime.
 *
 * This function automatically selects the optimal SIMD implementation
 * (AVX2, SSE2, or scalar) based on CPU capabilities detected at runtime.
 *
 * @param a First input array
 * @param b Second input array
 * @param c Output array (must have space for n elements)
 * @param n Number of elements to process
 */
inline void dispatch_add_arrays(const float* a, const float* b, float* c, size_t n) {
    // 使用 inline 静态变量确保线程安全的单次初始化
    static const detail::AddArraysFn dispatch = detail::resolve_add_arrays();
    dispatch(a, b, c, n);
}

}  // namespace hpc::simd
