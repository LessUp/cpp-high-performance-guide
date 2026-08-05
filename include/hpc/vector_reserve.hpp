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
#include <new>
#include <vector>

#include <hpc/instrumentation.hpp>

namespace hpc::vector_reserve {

//------------------------------------------------------------------------------
// Counting allocator to track allocations
//------------------------------------------------------------------------------

/**
 * @brief Custom allocator that tracks allocation counts via injected metrics.
 *
 * Replaces the previous global-static design with an injectable
 * instrumentation::OperationMetrics pointer. When metrics is nullptr,
 * the allocator behaves like a plain wrapper around std::malloc/free
 * with zero observation overhead.
 *
 * @tparam T The type of objects to allocate
 *
 * @warning The metrics pointer is non-owning. The pointed-to OperationMetrics
 *          must outlive every vector using this allocator: allocators propagate
 *          on copy/move/swap, so a dangling metrics pointer is UB.
 */
template <typename T>
class CountingAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    CountingAllocator() noexcept = default;

    explicit CountingAllocator(instrumentation::OperationMetrics* metrics) noexcept
        : metrics_(metrics) {}

    template <typename U>
    CountingAllocator(const CountingAllocator<U>& other) noexcept : metrics_(other.metrics()) {}

    T* allocate(std::size_t n) {
        // Guard against n * sizeof(T) overflowing size_t: malloc would
        // otherwise silently allocate a much smaller buffer than requested.
        if (n > std::size_t(-1) / sizeof(T)) {
            throw std::bad_alloc();
        }
        const std::size_t bytes = n * sizeof(T);
        if (metrics_) {
            metrics_->record_allocation(bytes);
        }
        // malloc(0) is implementation-defined; allocate 1 byte so the result
        // is always a valid non-null pointer that deallocate() can accept.
        void* ptr = std::malloc(bytes == 0 ? 1 : bytes);
        if (ptr == nullptr) {
            throw std::bad_alloc();  // Allocators must never return nullptr
        }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* ptr, std::size_t n) noexcept {
        if (metrics_) {
            metrics_->record_deallocation(n * sizeof(T));
        }
        std::free(ptr);
    }

    instrumentation::OperationMetrics* metrics() const noexcept { return metrics_; }

    template <typename U>
    struct rebind {
        using other = CountingAllocator<U>;
    };

private:
    instrumentation::OperationMetrics* metrics_ = nullptr;

    template <typename U>
    friend class CountingAllocator;
};

template <typename T, typename U>
bool operator==(const CountingAllocator<T>& lhs, const CountingAllocator<U>& rhs) noexcept {
    return lhs.metrics() == rhs.metrics();
}

template <typename T, typename U>
bool operator!=(const CountingAllocator<T>& lhs, const CountingAllocator<U>& rhs) noexcept {
    return !(lhs == rhs);
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
