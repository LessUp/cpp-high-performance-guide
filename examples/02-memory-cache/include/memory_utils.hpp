#pragma once
/**
 * @file memory_utils.hpp
 * @brief Memory and cache optimization utilities
 * 
 * This header provides utilities for memory alignment, cache-friendly
 * data structures, and performance measurement helpers.
 */

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <new>
#include <vector>

namespace hpc::memory {

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

/// Typical cache line size on modern x86 processors
constexpr std::size_t CACHE_LINE_SIZE = 64;

/// Page size on most systems
constexpr std::size_t PAGE_SIZE = 4096;

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

/**
 * @brief Custom deleter for aligned memory
 */
struct AlignedDeleter {
    void operator()(void* ptr) const {
        aligned_free(ptr);
    }
};

/**
 * @brief Unique pointer with aligned memory
 */
template<typename T>
using aligned_unique_ptr = std::unique_ptr<T, AlignedDeleter>;

/**
 * @brief Create aligned unique pointer
 */
template<typename T>
aligned_unique_ptr<T> make_aligned(std::size_t count, std::size_t alignment = CACHE_LINE_SIZE) {
    void* ptr = aligned_alloc(count * sizeof(T), alignment);
    if (!ptr) {
        throw std::bad_alloc();
    }
    return aligned_unique_ptr<T>(static_cast<T*>(ptr));
}

//------------------------------------------------------------------------------
// Aligned Allocator for STL containers
//------------------------------------------------------------------------------

/**
 * @brief STL-compatible allocator with custom alignment
 */
template<typename T, std::size_t Alignment = CACHE_LINE_SIZE>
class AlignedAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    static constexpr std::size_t alignment = Alignment;
    
    AlignedAllocator() noexcept = default;
    
    template<typename U>
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
    
    void deallocate(T* ptr, std::size_t) noexcept {
        aligned_free(ptr);
    }
    
    template<typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };
};

template<typename T, typename U, std::size_t A>
bool operator==(const AlignedAllocator<T, A>&, const AlignedAllocator<U, A>&) noexcept {
    return true;
}

template<typename T, typename U, std::size_t A>
bool operator!=(const AlignedAllocator<T, A>&, const AlignedAllocator<U, A>&) noexcept {
    return false;
}

/**
 * @brief Vector with cache-line aligned storage
 */
template<typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, CACHE_LINE_SIZE>>;

//------------------------------------------------------------------------------
// Cache Line Padding
//------------------------------------------------------------------------------

/**
 * @brief Pad a type to cache line size to prevent false sharing
 */
template<typename T>
struct alignas(CACHE_LINE_SIZE) CacheLinePadded {
    T value;
    
    CacheLinePadded() = default;
    explicit CacheLinePadded(const T& v) : value(v) {}
    explicit CacheLinePadded(T&& v) : value(std::move(v)) {}
    
    operator T&() { return value; }
    operator const T&() const { return value; }
    
    T* operator->() { return &value; }
    const T* operator->() const { return &value; }
};

//------------------------------------------------------------------------------
// Prefetch Hints
//------------------------------------------------------------------------------

/**
 * @brief Prefetch data for reading
 */
template<typename T>
inline void prefetch_read(const T* ptr) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ptr, 0, 3);  // Read, high temporal locality
#elif defined(_MSC_VER)
    _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0);
#endif
}

/**
 * @brief Prefetch data for writing
 */
template<typename T>
inline void prefetch_write(T* ptr) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ptr, 1, 3);  // Write, high temporal locality
#elif defined(_MSC_VER)
    _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0);
#endif
}

/**
 * @brief Prefetch with specified locality hint
 * @param locality 0 = non-temporal, 3 = high temporal locality
 */
template<typename T>
inline void prefetch(const T* ptr, int locality = 3) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ptr, 0, locality);
#elif defined(_MSC_VER)
    switch (locality) {
        case 0: _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_NTA); break;
        case 1: _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T2); break;
        case 2: _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T1); break;
        default: _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0); break;
    }
#endif
}

} // namespace hpc::memory
