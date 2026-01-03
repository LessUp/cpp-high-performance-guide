#pragma once

/**
 * @file simd_utils.hpp
 * @brief SIMD utility functions and feature detection
 * 
 * This header provides common utilities for SIMD programming including
 * feature detection, alignment helpers, and basic SIMD operations.
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <cstdlib>
#include <new>

// Feature detection macros
#ifdef __SSE2__
    #define HPC_HAS_SSE2 1
    #include <emmintrin.h>
#endif

#ifdef __AVX__
    #define HPC_HAS_AVX 1
    #include <immintrin.h>
#endif

#ifdef __AVX2__
    #define HPC_HAS_AVX2 1
    #include <immintrin.h>
#endif

#ifdef __AVX512F__
    #define HPC_HAS_AVX512 1
    #include <immintrin.h>
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
 */
template<typename T>
class aligned_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template<typename U>
    struct rebind {
        using other = aligned_allocator<U>;
    };

    aligned_allocator() = default;
    
    template<typename U>
    aligned_allocator(const aligned_allocator<U>&) {}

    pointer allocate(size_type n) {
        const size_t alignment = get_simd_alignment();
        const size_t size = n * sizeof(T);
        
        void* ptr = nullptr;
        if (posix_memalign(&ptr, alignment, size) != 0) {
            throw std::bad_alloc();
        }
        return static_cast<pointer>(ptr);
    }

    void deallocate(pointer p, size_type) {
        free(p);
    }

    template<typename U>
    bool operator==(const aligned_allocator<U>&) const { return true; }
    
    template<typename U>
    bool operator!=(const aligned_allocator<U>&) const { return false; }
};

/**
 * @brief Aligned vector type for SIMD operations
 */
template<typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

/**
 * @brief Aligned buffer type alias for compatibility
 */
template<typename T>
using AlignedBuffer = aligned_vector<T>;

/**
 * @brief Create an aligned vector with the specified size
 */
template<typename T>
aligned_vector<T> make_aligned_vector(size_t size) {
    return aligned_vector<T>(size);
}

/**
 * @brief Create an aligned vector with the specified size and initial value
 */
template<typename T>
aligned_vector<T> make_aligned_vector(size_t size, const T& value) {
    return aligned_vector<T>(size, value);
}

/**
 * @brief SIMD capability levels
 */
enum class SIMDLevel {
    Scalar,
    SSE2,
    AVX,
    AVX2,
    AVX512
};

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
        case SIMDLevel::AVX512: return "AVX-512";
        case SIMDLevel::AVX2:   return "AVX2";
        case SIMDLevel::AVX:    return "AVX";
        case SIMDLevel::SSE2:   return "SSE2";
        case SIMDLevel::Scalar: return "Scalar";
        default:                return "Unknown";
    }
}

/**
 * @brief Get the vector width in bytes for a SIMD level
 */
inline size_t simd_vector_width(SIMDLevel level) {
    switch (level) {
        case SIMDLevel::AVX512: return 64;
        case SIMDLevel::AVX2:   return 32;
        case SIMDLevel::AVX:    return 32;
        case SIMDLevel::SSE2:   return 16;
        case SIMDLevel::Scalar: return sizeof(float);
        default:                return sizeof(float);
    }
}

/**
 * @brief Add two arrays using SIMD wrapper
 */
void add_arrays_wrapped(const float* a, const float* b, float* c, size_t n);

/**
 * @brief Compute dot product using SIMD wrapper
 */
float dot_product_wrapped(const float* a, const float* b, size_t n);

/**
 * @brief Scale array using SIMD wrapper
 */
void scale_array_wrapped(float* arr, float scalar, size_t n);

/**
 * @brief Clamp array values using SIMD wrapper
 */
void clamp_array_wrapped(float* arr, float min_val, float max_val, size_t n);

} // namespace hpc::simd