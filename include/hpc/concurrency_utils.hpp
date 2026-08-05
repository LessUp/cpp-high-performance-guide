/**
 * @file concurrency_utils.hpp
 * @brief Concurrency and multithreading utilities
 *
 * This header provides utilities for concurrent programming including
 * atomic operations, spin locks, and parallel execution helpers.
 *
 * Validates:
 *   - Requirement 5.1: Atomic Memory Ordering
 *   - Requirement 5.2: Lock-Free Data Structures
 *   - Requirement 5.3: False Sharing Detection
 *   - Requirement 5.4: OpenMP Integration
 */

#pragma once

#include <atomic>
#include <chrono>
#include <functional>    // for std::ref in run_parallel
#include <hpc/core.hpp>  // platform constants
#include <thread>
#include <vector>

namespace hpc::concurrency {

//------------------------------------------------------------------------------
// Constants and utilities from the canonical core header
//------------------------------------------------------------------------------

// CACHE_LINE_SIZE comes from the hpc::core namespace
// hardware_concurrency() is defined in hpc::core

// Backward-compatibility alias in the current namespace
using hpc::core::hardware_concurrency;

//------------------------------------------------------------------------------
// Aligned Atomic Counter
//------------------------------------------------------------------------------

/**
 * @brief Cache-line aligned atomic counter to avoid false sharing
 *
 * This is equivalent to CacheLinePadded<std::atomic<int64_t>> but provides
 * convenience methods for common atomic operations.
 *
 * For other types requiring cache-line alignment, use:
 * - hpc::memory::CacheLinePadded<T> (from memory_utils.hpp)
 * - alignas(hpc::core::CACHE_LINE_SIZE) directly
 *
 * Note: The UnalignedCounter type has been removed. For unaligned counters,
 * use std::atomic<int64_t> directly to demonstrate the performance difference.
 */
struct alignas(hpc::core::CACHE_LINE_SIZE) AlignedCounter {
    std::atomic<int64_t> value{0};

    void increment(std::memory_order order = std::memory_order_seq_cst) {
        value.fetch_add(1, order);
    }

    int64_t load(std::memory_order order = std::memory_order_seq_cst) const {
        return value.load(order);
    }

    void store(int64_t v, std::memory_order order = std::memory_order_seq_cst) {
        value.store(v, order);
    }
};

//------------------------------------------------------------------------------
// Spin Lock - satisfies C++ BasicLockable and Lockable concepts
//------------------------------------------------------------------------------

/**
 * @brief Simple spin lock using atomic flag
 *
 * This class satisfies both BasicLockable and Lockable C++ named requirements,
 * allowing it to be used with std::lock_guard, std::unique_lock,
 * and std::scoped_lock.
 *
 * BasicLockable requirements:
 * - lock() blocks until the lock is acquired
 * - unlock() releases the lock
 *
 * Lockable requirements (additional):
 * - try_lock() attempts to acquire the lock without blocking
 *
 * @note Spin locks are appropriate for very short critical sections
 *       where the overhead of mutex operations would be significant.
 *       For longer critical sections, prefer std::mutex.
 */
class SpinLock {
public:
    SpinLock() noexcept = default;

    // Non-copyable and non-movable
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

    /// Acquire the lock (blocks until acquired)
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
// Spin
#if defined(__cpp_lib_atomic_flag_test)
            while (flag_.test(std::memory_order_relaxed)) {
                // Reduce cache line bouncing
            }
#endif
        }
    }

    /// Release the lock
    void unlock() { flag_.clear(std::memory_order_release); }

    /// Try to acquire the lock without blocking
    /// @return true if the lock was acquired, false otherwise
    bool try_lock() { return !flag_.test_and_set(std::memory_order_acquire); }

private:
    std::atomic_flag flag_;  // C++20: default constructor leaves the flag clear
};

//------------------------------------------------------------------------------
// Parallel Execution Helper
//------------------------------------------------------------------------------

/**
 * @brief Run a callable concurrently on multiple threads and measure the time
 *
 * @param func Callable invoked as func(thread_id) with thread_id in
 *             [0, num_threads). All invocations run concurrently, so func
 *             must be safe to call from multiple threads at once.
 * @param num_threads Number of threads to spawn
 * @return Elapsed wall-clock time in milliseconds
 */
template <typename Func>
double run_parallel(Func&& func, unsigned int num_threads) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    // Materialize the callable once. Forwarding Func inside the loop would,
    // for rvalue arguments, move from it on the first iteration and leave the
    // remaining threads with a moved-from callable. std::ref shares the single
    // instance instead of copying it into every thread (also works for
    // move-only callables).
    auto task = std::forward<Func>(func);

    auto start = std::chrono::steady_clock::now();

    for (unsigned int i = 0; i < num_threads; ++i) {
        try {
            threads.emplace_back(std::ref(task), i);
        } catch (...) {
            // Join already-started threads before unwinding: destroying a
            // joinable std::thread would call std::terminate.
            for (auto& t : threads) {
                t.join();
            }
            throw;
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

}  // namespace hpc::concurrency
