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

#include "hpc/core.hpp"  // 平台常量

#include <atomic>
#include <chrono>
#include <concepts>  // C++20 concepts
#include <functional>
#include <mutex>  // for std::lock_guard, std::unique_lock
#include <thread>
#include <vector>

namespace hpc::concurrency {

//------------------------------------------------------------------------------
// 使用核心头文件中的常量和函数
//------------------------------------------------------------------------------

// CACHE_LINE_SIZE 已通过 hpc/core.hpp 的 using 声明导入
// hardware_concurrency() 在 hpc::core 中定义

// 为了向后兼容，在当前命名空间中提供别名
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
 * - alignas(CACHE_LINE_SIZE) directly
 *
 * Note: The UnalignedCounter type has been removed. For unaligned counters,
 * use std::atomic<int64_t> directly to demonstrate the performance difference.
 */
struct alignas(CACHE_LINE_SIZE) AlignedCounter {
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
// Spin Lock - 符合 C++ BasicLockable 和 Lockable 概念
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
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

//------------------------------------------------------------------------------
// Parallel Execution Helper
//------------------------------------------------------------------------------

/// Run a function on multiple threads and measure time
template <typename Func>
double run_parallel(Func&& func, unsigned int num_threads) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(std::forward<Func>(func), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

}  // namespace hpc::concurrency
