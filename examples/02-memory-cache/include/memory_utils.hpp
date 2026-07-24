/**
 * @file memory_utils.hpp
 * @brief Memory and cache optimization utilities
 *
 * This header provides utilities for memory alignment and cache-friendly
 * data structures.
 *
 * Validates:
 *   - Requirement 2.1: AOS vs SOA Comparison
 *   - Requirement 2.2: False Sharing Demonstration
 *   - Requirement 2.3: Memory Alignment for SIMD
 */

#pragma once

#include <cstddef>
#include <cstdlib>
#include <hpc/core.hpp>  // platform constants
#include <memory>
#include <new>
#include <vector>

namespace hpc::memory {

//------------------------------------------------------------------------------
// Constants from the canonical core header
//------------------------------------------------------------------------------

// CACHE_LINE_SIZE and PAGE_SIZE come from the hpc::core namespace

//------------------------------------------------------------------------------
// Aligned Memory Allocation
//------------------------------------------------------------------------------

/**
 * @brief Allocate aligned memory
 * @param size Number of bytes to allocate
 * @param alignment Alignment requirement (must be power of 2)
 * @return Pointer to aligned memory, or nullptr on failure
 */
inline void* aligned_alloc(std::size_t size, std::size_t alignment) {
#if defined(_MSC_VER)
    return _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return nullptr;
    }
    return ptr;
#endif
}

/**
 * @brief Free aligned memory
 * @param ptr Pointer to aligned memory
 */
inline void aligned_free(void* ptr) {
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

template <typename T>
struct AlignedDeleter {
    std::size_t count = 0;

    void operator()(T* ptr) const noexcept {
        if (ptr == nullptr) {
            return;
        }
        std::destroy_n(ptr, count);
        aligned_free(ptr);
    }
};

/**
 * @brief Unique pointer with aligned memory
 */
template <typename T>
using aligned_unique_ptr = std::unique_ptr<T[], AlignedDeleter<T>>;

/**
 * @brief Create aligned unique pointer
 */
template <typename T>
aligned_unique_ptr<T> make_aligned(std::size_t count,
                                   std::size_t alignment = hpc::core::CACHE_LINE_SIZE) {
    if (count == 0) {
        return aligned_unique_ptr<T>(nullptr, AlignedDeleter<T>{});
    }

    void* ptr = aligned_alloc(count * sizeof(T), alignment);
    if (!ptr) {
        throw std::bad_alloc();
    }

    T* typed_ptr = static_cast<T*>(ptr);
    try {
        std::uninitialized_default_construct_n(typed_ptr, count);
    } catch (...) {
        aligned_free(typed_ptr);
        throw;
    }

    return aligned_unique_ptr<T>(typed_ptr, AlignedDeleter<T>{count});
}

//------------------------------------------------------------------------------
// Aligned Allocator for STL containers
//------------------------------------------------------------------------------

/**
 * @brief Cache-line aligned allocator for STL containers
 *
 * Uses a compile-time constant alignment (default: CACHE_LINE_SIZE).
 * Designed for false-sharing elimination in multi-threaded code.
 *
 * See docs/META.md: cache-line allocator for the domain rationale.
 * For SIMD-width alignment, see hpc::simd::AlignedAllocator in include/hpc/simd.hpp.
 */
template <typename T, std::size_t Alignment = hpc::core::CACHE_LINE_SIZE>
class AlignedAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr std::size_t alignment = Alignment;

    AlignedAllocator() noexcept = default;

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    T* allocate(std::size_t n) {
        if (n > std::size_t(-1) / sizeof(T)) {
            throw std::bad_alloc();
        }

        void* ptr = aligned_alloc(n * sizeof(T), Alignment);
        if (!ptr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* ptr, std::size_t) noexcept { aligned_free(ptr); }

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };
};

template <typename T, typename U, std::size_t A>
bool operator==(const AlignedAllocator<T, A>&, const AlignedAllocator<U, A>&) noexcept {
    return true;
}

template <typename T, typename U, std::size_t A>
bool operator!=(const AlignedAllocator<T, A>&, const AlignedAllocator<U, A>&) noexcept {
    return false;
}

/**
 * @brief Vector with cache-line aligned storage
 */
template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, hpc::core::CACHE_LINE_SIZE>>;

//------------------------------------------------------------------------------
// Cache Line Padding
//------------------------------------------------------------------------------

/**
 * @brief Pad a type to cache line size to prevent false sharing
 */
template <typename T>
struct alignas(hpc::core::CACHE_LINE_SIZE) CacheLinePadded {
    T value;

    CacheLinePadded() = default;
    explicit CacheLinePadded(const T& v) : value(v) {}
    explicit CacheLinePadded(T&& v) : value(std::move(v)) {}

    operator T&() { return value; }
    operator const T&() const { return value; }

    T* operator->() { return &value; }
    const T* operator->() const { return &value; }
};

}  // namespace hpc::memory
