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
        if (metrics_) {
            metrics_->record_allocation(n * sizeof(T));
        }
        return static_cast<T*>(std::malloc(n * sizeof(T)));
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
