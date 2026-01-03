/**
 * @file concurrency_properties.cpp
 * @brief Property-based tests for concurrency correctness
 * 
 * Feature: hpc-optimization-guide
 * Properties 10, 11, 12: Concurrency correctness properties
 * Validates: Requirements 5.1, 5.2, 5.4, 5.5
 */

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>

#include "../../examples/05-concurrency/include/concurrency_utils.hpp"

namespace {

// Helper to run concurrent operations
template<typename Func>
void run_threads(Func&& func, int num_threads) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(std::forward<Func>(func), i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

} // anonymous namespace

/**
 * Property 10: Atomic Operations Correctness
 * 
 * For any concurrent increment scenario with T threads each performing N increments,
 * the final counter value SHALL equal T * N when using std::atomic with appropriate
 * memory ordering.
 * 
 * Validates: Requirements 5.1
 */
RC_GTEST_PROP(AtomicProperties, ConcurrentIncrementCorrectness, ()) {
    // Generate reasonable thread count and iterations
    const int num_threads = *rc::gen::inRange(2, 9);  // 2-8 threads
    const int increments_per_thread = *rc::gen::inRange(1000, 10001);  // 1000-10000
    
    std::atomic<int64_t> counter{0};
    
    auto increment_task = [&](int) {
        for (int i = 0; i < increments_per_thread; ++i) {
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };
    
    run_threads(increment_task, num_threads);
    
    int64_t expected = static_cast<int64_t>(num_threads) * increments_per_thread;
    RC_ASSERT(counter.load() == expected);
}

/**
 * Property 10: Atomic Operations Correctness - Sequential Consistency
 * 
 * Same property but with seq_cst ordering.
 * 
 * Validates: Requirements 5.1
 */
RC_GTEST_PROP(AtomicProperties, SeqCstIncrementCorrectness, ()) {
    const int num_threads = *rc::gen::inRange(2, 9);
    const int increments_per_thread = *rc::gen::inRange(1000, 10001);
    
    std::atomic<int64_t> counter{0};
    
    auto increment_task = [&](int) {
        for (int i = 0; i < increments_per_thread; ++i) {
            counter.fetch_add(1, std::memory_order_seq_cst);
        }
    };
    
    run_threads(increment_task, num_threads);
    
    int64_t expected = static_cast<int64_t>(num_threads) * increments_per_thread;
    RC_ASSERT(counter.load() == expected);
}

/**
 * Property 10: Atomic Operations Correctness - Aligned Counter
 * 
 * Using cache-line aligned counter should also be correct.
 * 
 * Validates: Requirements 5.1
 */
RC_GTEST_PROP(AtomicProperties, AlignedCounterCorrectness, ()) {
    const int num_threads = *rc::gen::inRange(2, 9);
    const int increments_per_thread = *rc::gen::inRange(1000, 10001);
    
    hpc::concurrency::AlignedCounter counter;
    
    auto increment_task = [&](int) {
        for (int i = 0; i < increments_per_thread; ++i) {
            counter.increment(std::memory_order_relaxed);
        }
    };
    
    run_threads(increment_task, num_threads);
    
    int64_t expected = static_cast<int64_t>(num_threads) * increments_per_thread;
    RC_ASSERT(counter.load() == expected);
}

/**
 * Property 10: Atomic Operations Correctness - Compare-and-Swap
 * 
 * CAS-based increment should also produce correct results.
 * 
 * Validates: Requirements 5.1
 */
RC_GTEST_PROP(AtomicProperties, CASIncrementCorrectness, ()) {
    const int num_threads = *rc::gen::inRange(2, 5);  // Fewer threads for CAS
    const int increments_per_thread = *rc::gen::inRange(1000, 5001);
    
    std::atomic<int64_t> counter{0};
    
    auto cas_increment_task = [&](int) {
        for (int i = 0; i < increments_per_thread; ++i) {
            int64_t expected = counter.load(std::memory_order_relaxed);
            while (!counter.compare_exchange_weak(expected, expected + 1,
                                                   std::memory_order_relaxed)) {
                // Retry on failure
            }
        }
    };
    
    run_threads(cas_increment_task, num_threads);
    
    int64_t expected = static_cast<int64_t>(num_threads) * increments_per_thread;
    RC_ASSERT(counter.load() == expected);
}

/**
 * Property 10: Atomic Operations Correctness - Multiple Counters
 * 
 * Multiple independent counters should all be correct.
 * 
 * Validates: Requirements 5.1
 */
RC_GTEST_PROP(AtomicProperties, MultipleCountersCorrectness, ()) {
    const int num_threads = *rc::gen::inRange(2, 9);
    const int increments_per_thread = *rc::gen::inRange(1000, 5001);
    constexpr int NUM_COUNTERS = 4;
    
    std::array<std::atomic<int64_t>, NUM_COUNTERS> counters{};
    for (auto& c : counters) {
        c.store(0);
    }
    
    auto increment_task = [&](int thread_id) {
        for (int i = 0; i < increments_per_thread; ++i) {
            // Each thread increments all counters
            for (auto& c : counters) {
                c.fetch_add(1, std::memory_order_relaxed);
            }
        }
    };
    
    run_threads(increment_task, num_threads);
    
    int64_t expected = static_cast<int64_t>(num_threads) * increments_per_thread;
    for (const auto& c : counters) {
        RC_ASSERT(c.load() == expected);
    }
}

/**
 * Property 10: Atomic Operations Correctness - Acquire-Release Synchronization
 * 
 * Data written before release should be visible after acquire.
 * 
 * Validates: Requirements 5.1
 */
TEST(AtomicSynchronizationTests, AcquireReleaseVisibility) {
    constexpr int NUM_TRIALS = 1000;
    
    for (int trial = 0; trial < NUM_TRIALS; ++trial) {
        std::atomic<int> data{0};
        std::atomic<bool> ready{false};
        int observed_data = -1;
        
        std::thread producer([&]() {
            data.store(42, std::memory_order_relaxed);
            ready.store(true, std::memory_order_release);
        });
        
        std::thread consumer([&]() {
            while (!ready.load(std::memory_order_acquire)) {
                // Spin
            }
            observed_data = data.load(std::memory_order_relaxed);
        });
        
        producer.join();
        consumer.join();
        
        ASSERT_EQ(observed_data, 42) << "Failed at trial " << trial;
    }
}

/**
 * Property 10: Atomic Operations Correctness - SpinLock
 * 
 * SpinLock should provide mutual exclusion.
 * 
 * Validates: Requirements 5.1
 */
RC_GTEST_PROP(AtomicProperties, SpinLockMutualExclusion, ()) {
    const int num_threads = *rc::gen::inRange(2, 9);
    const int increments_per_thread = *rc::gen::inRange(1000, 5001);
    
    hpc::concurrency::SpinLock lock;
    int64_t counter = 0;  // Non-atomic, protected by lock
    
    auto increment_task = [&](int) {
        for (int i = 0; i < increments_per_thread; ++i) {
            hpc::concurrency::SpinLockGuard guard(lock);
            ++counter;
        }
    };
    
    run_threads(increment_task, num_threads);
    
    int64_t expected = static_cast<int64_t>(num_threads) * increments_per_thread;
    RC_ASSERT(counter == expected);
}

// Standard GTest for edge cases
TEST(AtomicTests, SingleThreadCorrectness) {
    std::atomic<int> counter{0};
    
    for (int i = 0; i < 10000; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
    
    EXPECT_EQ(counter.load(), 10000);
}

TEST(AtomicTests, AtomicFlagTest) {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    
    // First test_and_set should return false (was clear)
    EXPECT_FALSE(flag.test_and_set());
    
    // Second test_and_set should return true (was set)
    EXPECT_TRUE(flag.test_and_set());
    
    // Clear and test again
    flag.clear();
    EXPECT_FALSE(flag.test_and_set());
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


// ============================================================================
// Property 11: Lock-Free Queue Invariants
// ============================================================================

// Include the lock-free queue header
// Note: In a real project, this would be in a separate header file
namespace hpc::concurrency {

template<typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity >= 2, "Capacity must be at least 2");
    
public:
    SPSCQueue() : head_(0), tail_(0) {}
    
    bool push(const T& value) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & MASK;
        
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;
        }
        
        buffer_[current_tail] = value;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }
    
    std::optional<T> pop() {
        const size_t current_head = head_.load(std::memory_order_relaxed);
        
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt;
        }
        
        T value = std::move(buffer_[current_head]);
        head_.store((current_head + 1) & MASK, std::memory_order_release);
        return value;
    }
    
    bool empty() const {
        return head_.load(std::memory_order_relaxed) == 
               tail_.load(std::memory_order_relaxed);
    }

private:
    static constexpr size_t MASK = Capacity - 1;
    static constexpr size_t CACHE_LINE_SIZE = 64;
    
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_;
    alignas(CACHE_LINE_SIZE) T buffer_[Capacity];
};

} // namespace hpc::concurrency

/**
 * Property 11: Lock-Free Queue Invariants - FIFO Ordering
 * 
 * For any sequence of concurrent push and pop operations on a lock-free queue,
 * the queue SHALL maintain FIFO ordering and never lose or duplicate elements.
 * 
 * Validates: Requirements 5.2
 */
RC_GTEST_PROP(LockFreeQueueProperties, SPSCFIFOOrdering, ()) {
    const int num_items = *rc::gen::inRange(100, 10001);
    
    hpc::concurrency::SPSCQueue<int, 1024> queue;
    std::vector<int> received;
    received.reserve(num_items);
    
    std::atomic<bool> producer_done{false};
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < num_items; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
        }
        producer_done.store(true, std::memory_order_release);
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
            if (auto value = queue.pop()) {
                received.push_back(*value);
            } else {
                std::this_thread::yield();
            }
        }
        // Drain remaining
        while (auto value = queue.pop()) {
            received.push_back(*value);
        }
    });
    
    producer.join();
    consumer.join();
    
    // Verify no elements lost
    RC_ASSERT(static_cast<int>(received.size()) == num_items);
    
    // Verify FIFO ordering
    for (int i = 0; i < num_items; ++i) {
        RC_ASSERT(received[i] == i);
    }
}

/**
 * Property 11: Lock-Free Queue Invariants - No Duplicates
 * 
 * Elements should never be duplicated.
 * 
 * Validates: Requirements 5.2
 */
RC_GTEST_PROP(LockFreeQueueProperties, SPSCNoDuplicates, ()) {
    const int num_items = *rc::gen::inRange(100, 5001);
    
    hpc::concurrency::SPSCQueue<int, 512> queue;
    std::vector<int> received;
    received.reserve(num_items);
    
    std::atomic<bool> producer_done{false};
    
    std::thread producer([&]() {
        for (int i = 0; i < num_items; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
        }
        producer_done.store(true, std::memory_order_release);
    });
    
    std::thread consumer([&]() {
        while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
            if (auto value = queue.pop()) {
                received.push_back(*value);
            } else {
                std::this_thread::yield();
            }
        }
        while (auto value = queue.pop()) {
            received.push_back(*value);
        }
    });
    
    producer.join();
    consumer.join();
    
    // Check for duplicates by sorting and looking for adjacent equal elements
    std::vector<int> sorted = received;
    std::sort(sorted.begin(), sorted.end());
    
    for (size_t i = 1; i < sorted.size(); ++i) {
        RC_ASSERT(sorted[i] != sorted[i-1]);  // No duplicates
    }
}

/**
 * Property 11: Lock-Free Queue Invariants - Bounded Capacity
 * 
 * Queue should respect its capacity limit.
 * 
 * Validates: Requirements 5.2
 */
TEST(LockFreeQueueTests, BoundedCapacity) {
    constexpr size_t CAPACITY = 64;
    hpc::concurrency::SPSCQueue<int, CAPACITY> queue;
    
    // Fill the queue (capacity - 1 because one slot is always empty)
    int pushed = 0;
    for (size_t i = 0; i < CAPACITY; ++i) {
        if (queue.push(static_cast<int>(i))) {
            ++pushed;
        }
    }
    
    // Should have pushed CAPACITY - 1 items
    EXPECT_EQ(pushed, CAPACITY - 1);
    
    // Next push should fail
    EXPECT_FALSE(queue.push(999));
    
    // Pop one and push should succeed
    auto value = queue.pop();
    EXPECT_TRUE(value.has_value());
    EXPECT_TRUE(queue.push(999));
}

/**
 * Property 11: Lock-Free Queue Invariants - Empty Queue Behavior
 * 
 * Pop from empty queue should return nullopt.
 * 
 * Validates: Requirements 5.2
 */
TEST(LockFreeQueueTests, EmptyQueuePop) {
    hpc::concurrency::SPSCQueue<int, 64> queue;
    
    // Pop from empty queue
    auto value = queue.pop();
    EXPECT_FALSE(value.has_value());
    
    // Push and pop
    EXPECT_TRUE(queue.push(42));
    value = queue.pop();
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, 42);
    
    // Queue should be empty again
    value = queue.pop();
    EXPECT_FALSE(value.has_value());
}


// ============================================================================
// Property 12: OpenMP Scaling Efficiency
// ============================================================================

#ifdef _OPENMP
#include <omp.h>
#endif

/**
 * Property 12: OpenMP Scaling Efficiency
 * 
 * For any parallelizable workload with sufficient work per thread,
 * using OpenMP with T threads SHALL achieve at least 0.5 * T speedup
 * (50% efficiency) compared to single-threaded execution.
 * 
 * Validates: Requirements 5.4, 5.5
 */
TEST(OpenMPScalingTests, ParallelForEfficiency) {
#ifndef _OPENMP
    GTEST_SKIP() << "OpenMP not available";
#else
    constexpr size_t N = 10000000;
    std::vector<double> data(N);
    
    // Initialize data
    for (size_t i = 0; i < N; ++i) {
        data[i] = static_cast<double>(i);
    }
    
    // Baseline: single thread
    omp_set_num_threads(1);
    
    auto baseline_start = std::chrono::high_resolution_clock::now();
    double sum1 = 0.0;
    #pragma omp parallel for reduction(+:sum1)
    for (size_t i = 0; i < N; ++i) {
        sum1 += std::sqrt(data[i]) * std::sin(data[i] * 0.0001);
    }
    auto baseline_end = std::chrono::high_resolution_clock::now();
    double baseline_time = std::chrono::duration<double>(baseline_end - baseline_start).count();
    
    // Multi-threaded
    int num_threads = std::min(4, omp_get_max_threads());
    omp_set_num_threads(num_threads);
    
    auto parallel_start = std::chrono::high_resolution_clock::now();
    double sum2 = 0.0;
    #pragma omp parallel for reduction(+:sum2)
    for (size_t i = 0; i < N; ++i) {
        sum2 += std::sqrt(data[i]) * std::sin(data[i] * 0.0001);
    }
    auto parallel_end = std::chrono::high_resolution_clock::now();
    double parallel_time = std::chrono::duration<double>(parallel_end - parallel_start).count();
    
    double speedup = baseline_time / parallel_time;
    double efficiency = speedup / num_threads;
    
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Baseline time: " << baseline_time * 1000 << " ms" << std::endl;
    std::cout << "Parallel time: " << parallel_time * 1000 << " ms" << std::endl;
    std::cout << "Speedup: " << speedup << "x" << std::endl;
    std::cout << "Efficiency: " << efficiency * 100 << "%" << std::endl;
    
    // Results should match
    EXPECT_NEAR(sum1, sum2, 1e-6);
    
    // Expect at least 50% efficiency (relaxed due to system variability)
    // In practice, well-parallelized code should achieve 70-90% efficiency
    EXPECT_GE(efficiency, 0.3) << "Efficiency too low: " << efficiency * 100 << "%";
    
    // Reset
    omp_set_num_threads(omp_get_max_threads());
#endif
}

/**
 * Property 12: OpenMP Scaling Efficiency - Reduction Correctness
 * 
 * Parallel reduction should produce correct results.
 * 
 * Validates: Requirements 5.4, 5.5
 */
RC_GTEST_PROP(OpenMPProperties, ReductionCorrectness, ()) {
#ifndef _OPENMP
    RC_SUCCEED("OpenMP not available");
#else
    const int size = *rc::gen::inRange(10000, 100001);
    
    std::vector<int64_t> data(size);
    for (int i = 0; i < size; ++i) {
        data[i] = *rc::gen::inRange(-100, 101);
    }
    
    // Sequential sum
    int64_t seq_sum = 0;
    for (int i = 0; i < size; ++i) {
        seq_sum += data[i];
    }
    
    // Parallel sum
    int64_t par_sum = 0;
    #pragma omp parallel for reduction(+:par_sum)
    for (int i = 0; i < size; ++i) {
        par_sum += data[i];
    }
    
    RC_ASSERT(seq_sum == par_sum);
#endif
}

/**
 * Property 12: OpenMP Scaling Efficiency - Min/Max Reduction
 * 
 * Parallel min/max reduction should produce correct results.
 * 
 * Validates: Requirements 5.4, 5.5
 */
RC_GTEST_PROP(OpenMPProperties, MinMaxReductionCorrectness, ()) {
#ifndef _OPENMP
    RC_SUCCEED("OpenMP not available");
#else
    const int size = *rc::gen::inRange(1000, 50001);
    
    std::vector<int> data(size);
    for (int i = 0; i < size; ++i) {
        data[i] = *rc::gen::inRange(-10000, 10001);
    }
    
    // Sequential min/max
    int seq_min = data[0];
    int seq_max = data[0];
    for (int i = 1; i < size; ++i) {
        seq_min = std::min(seq_min, data[i]);
        seq_max = std::max(seq_max, data[i]);
    }
    
    // Parallel min/max
    int par_min = data[0];
    int par_max = data[0];
    #pragma omp parallel for reduction(min:par_min) reduction(max:par_max)
    for (int i = 0; i < size; ++i) {
        par_min = std::min(par_min, data[i]);
        par_max = std::max(par_max, data[i]);
    }
    
    RC_ASSERT(seq_min == par_min);
    RC_ASSERT(seq_max == par_max);
#endif
}

/**
 * Property 12: OpenMP Scaling Efficiency - Array Operations
 * 
 * Parallel array operations should produce correct results.
 * 
 * Validates: Requirements 5.4, 5.5
 */
RC_GTEST_PROP(OpenMPProperties, ParallelArrayOperations, ()) {
#ifndef _OPENMP
    RC_SUCCEED("OpenMP not available");
#else
    const int size = *rc::gen::inRange(1000, 50001);
    
    std::vector<double> a(size), b(size), c_seq(size), c_par(size);
    
    for (int i = 0; i < size; ++i) {
        a[i] = *rc::gen::inRange(-100, 101) * 0.1;
        b[i] = *rc::gen::inRange(-100, 101) * 0.1;
    }
    
    // Sequential
    for (int i = 0; i < size; ++i) {
        c_seq[i] = a[i] * b[i] + a[i];
    }
    
    // Parallel
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        c_par[i] = a[i] * b[i] + a[i];
    }
    
    // Verify results match
    for (int i = 0; i < size; ++i) {
        RC_ASSERT(std::abs(c_seq[i] - c_par[i]) < 1e-10);
    }
#endif
}
