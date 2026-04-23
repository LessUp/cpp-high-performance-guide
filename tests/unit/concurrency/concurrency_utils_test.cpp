/**
 * @file concurrency_utils_test.cpp
 * @brief Unit tests for concurrency_utils.hpp
 */

#include "concurrency_utils.hpp"

#include <gtest/gtest.h>

#include <numeric>

namespace hpc::concurrency::test {

// ---------------------------------------------------------------------------
// hardware_concurrency
// ---------------------------------------------------------------------------

TEST(HardwareConcurrencyTest, ReturnsPositive) {
    EXPECT_GE(hardware_concurrency(), 1u);
}

// ---------------------------------------------------------------------------
// AlignedCounter
// ---------------------------------------------------------------------------

TEST(AlignedCounterTest, SizeIsAtLeastCacheLine) {
    EXPECT_GE(sizeof(AlignedCounter), CACHE_LINE_SIZE);
}

TEST(AlignedCounterTest, AlignmentIsCacheLine) {
    EXPECT_EQ(alignof(AlignedCounter), CACHE_LINE_SIZE);
}

TEST(AlignedCounterTest, IncrementWorks) {
    AlignedCounter counter;
    counter.increment(std::memory_order_relaxed);
    counter.increment(std::memory_order_relaxed);
    EXPECT_EQ(counter.load(std::memory_order_relaxed), 2);
}

TEST(AlignedCounterTest, StoreLoadWorks) {
    AlignedCounter counter;
    counter.store(42);
    EXPECT_EQ(counter.load(), 42);
}

// ---------------------------------------------------------------------------
// SpinLock
// ---------------------------------------------------------------------------

TEST(SpinLockTest, LockUnlock) {
    SpinLock lock;
    lock.lock();
    lock.unlock();
    // Should not deadlock
}

TEST(SpinLockTest, TryLockSucceeds) {
    SpinLock lock;
    EXPECT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(SpinLockTest, TryLockFailsWhenHeld) {
    SpinLock lock;
    lock.lock();
    EXPECT_FALSE(lock.try_lock());
    lock.unlock();
}

TEST(SpinLockTest, GuardLocksAndUnlocks) {
    SpinLock lock;
    {
        SpinLockGuard guard(lock);
        EXPECT_FALSE(lock.try_lock());
    }
    // Lock should be released after guard goes out of scope
    EXPECT_TRUE(lock.try_lock());
    lock.unlock();
}

// ---------------------------------------------------------------------------
// run_parallel
// ---------------------------------------------------------------------------

TEST(RunParallelTest, AllThreadsRun) {
    std::atomic<int> counter{0};
    constexpr unsigned int NUM_THREADS = 4;

    run_parallel([&counter](unsigned int) { counter.fetch_add(1, std::memory_order_relaxed); },
                 NUM_THREADS);

    EXPECT_EQ(counter.load(), static_cast<int>(NUM_THREADS));
}

TEST(RunParallelTest, ThreadIdIsUnique) {
    constexpr unsigned int NUM_THREADS = 4;
    std::atomic<unsigned int> seen_mask{0};

    run_parallel(
        [&seen_mask](unsigned int thread_id) {
            seen_mask.fetch_or(1u << thread_id, std::memory_order_relaxed);
        },
        NUM_THREADS);

    // All bits 0..3 should be set
    unsigned int expected = (1u << NUM_THREADS) - 1;
    EXPECT_EQ(seen_mask.load(), expected);
}

// ---------------------------------------------------------------------------
// Concurrent increment correctness
// ---------------------------------------------------------------------------

TEST(ConcurrentIncrementTest, AtomicCounterIsCorrect) {
    AlignedCounter counter;
    constexpr unsigned int NUM_THREADS = 4;
    constexpr int ITERATIONS = 100000;

    run_parallel(
        [&counter](unsigned int) {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.increment(std::memory_order_relaxed);
            }
        },
        NUM_THREADS);

    EXPECT_EQ(counter.load(), static_cast<int64_t>(NUM_THREADS) * ITERATIONS);
}

}  // namespace hpc::concurrency::test
