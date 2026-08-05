/**
 * @file instrumentation.hpp
 * @brief Operation metrics collection with RAII scope management
 *
 * Replaces global static counters (e.g., Buffer::copy_count_,
 * CountingAllocator::allocation_count_) with an injectable, non-static
 * seam. Each test or demo creates a local OperationMetrics instance,
 * passes it to the module under observation, and uses the Scope helper
 * for automatic lifecycle management.
 *
 * Key concepts:
 * - Injectability: metrics are passed by pointer, not baked into class state
 * - RAII scope: OperationMetrics::Scope resets on construction, no manual cleanup
 * - Zero overhead: nullptr means "do not observe"
 * - Single-threaded observation: counters are plain (non-atomic) integers and
 *   are NOT thread-safe. Each Scope owns its own instance; if the same
 *   OperationMetrics is shared across threads, synchronization is the caller's
 *   responsibility (e.g. one metrics object per thread, merged afterwards).
 *
 * @example
 *   hpc::instrumentation::OperationMetrics metrics;
 *   hpc::instrumentation::OperationMetrics::Scope scope(metrics);
 *   Buffer buf(128, &metrics);
 *   Buffer copy(buf);  // metrics.copy_count == 1
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace hpc::instrumentation {

/**
 * @brief Generic operation counter for copy/move/allocation events.
 *
 * A single seam that replaces the scattered static counters previously
 * embedded in Buffer and CountingAllocator. By making the counter
 * external and injectable, tests no longer suffer from order-dependent
 * state pollution, and benchmarks pay zero overhead when metrics is
 * nullptr.
 */
class OperationMetrics {
public:
    size_t copy_count = 0;
    size_t move_count = 0;
    size_t allocation_count = 0;
    size_t deallocation_count = 0;
    size_t total_bytes_allocated = 0;
    size_t total_bytes_deallocated = 0;

    /**
     * @brief Reset all counters to zero.
     */
    void reset() noexcept {
        copy_count = 0;
        move_count = 0;
        allocation_count = 0;
        deallocation_count = 0;
        total_bytes_allocated = 0;
        total_bytes_deallocated = 0;
    }

    /**
     * @brief RAII scope that resets metrics on entry.
     *
     * Guarantees each test block starts from a clean slate without
     * manual reset_counts() calls.
     */
    class Scope {
    public:
        explicit Scope(OperationMetrics& m) : metrics_(m) { metrics_.reset(); }
        ~Scope() = default;

        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;
        Scope(Scope&&) = delete;
        Scope& operator=(Scope&&) = delete;

        OperationMetrics& metrics() noexcept { return metrics_; }
        const OperationMetrics& metrics() const noexcept { return metrics_; }

    private:
        OperationMetrics& metrics_;
    };

    // Named event recorders for type safety and locality
    void record_copy() noexcept { ++copy_count; }
    void record_move() noexcept { ++move_count; }
    void record_allocation(size_t bytes) noexcept {
        ++allocation_count;
        total_bytes_allocated += bytes;
    }
    void record_deallocation(size_t bytes) noexcept {
        ++deallocation_count;
        total_bytes_deallocated += bytes;
    }
};

}  // namespace hpc::instrumentation
