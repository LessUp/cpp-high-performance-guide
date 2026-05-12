/**
 * @file vector_reserve.hpp
 * @brief std::vector capacity management utilities
 *
 * This header provides utilities for understanding and optimizing
 * vector capacity management, particularly the reserve() operation.
 *
 * Key concepts:
 * - Vector growth strategy (typically 1.5x or 2x)
 * - reserve() vs resize()
 * - Counting allocations with custom allocator
 */

#pragma once

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <vector>

namespace hpc::vector_reserve {

//------------------------------------------------------------------------------
// Counting allocator to track allocations
//------------------------------------------------------------------------------

/**
 * @brief Custom allocator that tracks allocation counts
 *
 * This allocator is useful for demonstrating the impact of reserve()
 * on allocation patterns and for testing allocation behavior.
 *
 * @tparam T The type of objects to allocate
 */
template <typename T>
class CountingAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    CountingAllocator() noexcept = default;

    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        ++allocation_count_;
        total_bytes_allocated_ += n * sizeof(T);
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }

    void deallocate(T* ptr, std::size_t n) noexcept {
        ++deallocation_count_;
        total_bytes_deallocated_ += n * sizeof(T);
        std::free(ptr);
    }

    static void reset_counts() {
        allocation_count_ = 0;
        deallocation_count_ = 0;
        total_bytes_allocated_ = 0;
        total_bytes_deallocated_ = 0;
    }

    static size_t allocation_count_;
    static size_t deallocation_count_;
    static size_t total_bytes_allocated_;
    static size_t total_bytes_deallocated_;
};

template <typename T>
size_t CountingAllocator<T>::allocation_count_ = 0;

template <typename T>
size_t CountingAllocator<T>::deallocation_count_ = 0;

template <typename T>
size_t CountingAllocator<T>::total_bytes_allocated_ = 0;

template <typename T>
size_t CountingAllocator<T>::total_bytes_deallocated_ = 0;

template <typename T, typename U>
bool operator==(const CountingAllocator<T>&, const CountingAllocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const CountingAllocator<T>&, const CountingAllocator<U>&) noexcept {
    return false;
}

//------------------------------------------------------------------------------
// Type aliases
//------------------------------------------------------------------------------

/**
 * @brief Vector type that uses CountingAllocator
 */
template <typename T>
using CountingVector = std::vector<T, CountingAllocator<T>>;

}  // namespace hpc::vector_reserve
