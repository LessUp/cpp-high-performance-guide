/**
 * @file atomic_ordering.cpp
 * @brief Demonstrates std::atomic usage with different memory orderings
 * 
 * Memory orderings from weakest to strongest:
 * 1. memory_order_relaxed - No synchronization, only atomicity
 * 2. memory_order_consume - Deprecated, avoid using
 * 3. memory_order_acquire - Prevents reads from being reordered before
 * 4. memory_order_release - Prevents writes from being reordered after
 * 5. memory_order_acq_rel - Both acquire and release
 * 6. memory_order_seq_cst - Sequential consistency (default, strongest)
 */

#include "../include/concurrency_utils.hpp"
#include <iostream>
#include <cassert>
#include <array>

namespace hpc::concurrency {

// ============================================================================
// Example 1: Relaxed ordering - Only atomicity, no synchronization
// ============================================================================

/**
 * Relaxed ordering is the weakest. It only guarantees atomicity.
 * Use for counters where you don't need synchronization.
 */
void demonstrate_relaxed_ordering() {
    std::cout << "=== Relaxed Ordering ===" << std::endl;
    
    std::atomic<int> counter{0};
    constexpr int ITERATIONS = 100000;
    constexpr int NUM_THREADS = 4;
    
    auto increment = [&](unsigned int) {
        for (int i = 0; i < ITERATIONS; ++i) {
            // Relaxed: only atomicity, no ordering guarantees
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };
    
    double time_ms = run_parallel(increment, NUM_THREADS);
    
    std::cout << "Final counter value: " << counter.load() << std::endl;
    std::cout << "Expected value: " << ITERATIONS * NUM_THREADS << std::endl;
    std::cout << "Time: " << time_ms << " ms" << std::endl;
    
    // Relaxed ordering still guarantees atomicity
    assert(counter.load() == ITERATIONS * NUM_THREADS);
    std::cout << std::endl;
}

// ============================================================================
// Example 2: Acquire-Release ordering - Producer-Consumer pattern
// ============================================================================

/**
 * Acquire-Release is used for synchronization between threads.
 * - Release: All writes before this are visible to threads that acquire
 * - Acquire: All reads after this see writes from the releasing thread
 */
void demonstrate_acquire_release() {
    std::cout << "=== Acquire-Release Ordering ===" << std::endl;
    
    std::atomic<int> data{0};
    std::atomic<bool> ready{false};
    
    // Producer thread
    std::thread producer([&]() {
        data.store(42, std::memory_order_relaxed);  // Write data
        ready.store(true, std::memory_order_release);  // Release: make data visible
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        while (!ready.load(std::memory_order_acquire)) {  // Acquire: wait for data
            // Spin
        }
        // After acquire, we're guaranteed to see data = 42
        int value = data.load(std::memory_order_relaxed);
        std::cout << "Consumer read data: " << value << std::endl;
        assert(value == 42);
    });
    
    producer.join();
    consumer.join();
    
    std::cout << "Acquire-Release synchronization successful!" << std::endl;
    std::cout << std::endl;
}

// ============================================================================
// Example 3: Sequential Consistency - Strongest ordering
// ============================================================================

/**
 * Sequential consistency (seq_cst) is the default and strongest ordering.
 * All threads see all operations in the same order.
 * It's the easiest to reason about but may have performance overhead.
 */
void demonstrate_sequential_consistency() {
    std::cout << "=== Sequential Consistency ===" << std::endl;
    
    std::atomic<int> x{0}, y{0};
    std::atomic<int> r1{0}, r2{0};
    
    // This is the classic "store buffering" litmus test
    // With seq_cst, we cannot have both r1 == 0 and r2 == 0
    
    bool both_zero_found = false;
    
    for (int trial = 0; trial < 10000; ++trial) {
        x.store(0);
        y.store(0);
        r1.store(0);
        r2.store(0);
        
        std::thread t1([&]() {
            x.store(1, std::memory_order_seq_cst);
            r1.store(y.load(std::memory_order_seq_cst), std::memory_order_relaxed);
        });
        
        std::thread t2([&]() {
            y.store(1, std::memory_order_seq_cst);
            r2.store(x.load(std::memory_order_seq_cst), std::memory_order_relaxed);
        });
        
        t1.join();
        t2.join();
        
        // With seq_cst, at least one thread must see the other's store
        if (r1.load() == 0 && r2.load() == 0) {
            both_zero_found = true;
            break;
        }
    }
    
    if (both_zero_found) {
        std::cout << "WARNING: Both r1 and r2 were 0 (unexpected with seq_cst)" << std::endl;
    } else {
        std::cout << "Sequential consistency maintained across 10000 trials" << std::endl;
    }
    std::cout << std::endl;
}

// ============================================================================
// Example 4: Performance comparison of memory orderings
// ============================================================================

void benchmark_memory_orderings() {
    std::cout << "=== Memory Ordering Performance Comparison ===" << std::endl;
    
    constexpr int ITERATIONS = 1000000;
    constexpr int NUM_THREADS = 4;
    
    // Relaxed ordering
    {
        std::atomic<int64_t> counter{0};
        auto increment = [&](unsigned int) {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        };
        double time_ms = run_parallel(increment, NUM_THREADS);
        std::cout << "Relaxed:  " << time_ms << " ms" << std::endl;
    }
    
    // Acquire-Release ordering
    {
        std::atomic<int64_t> counter{0};
        auto increment = [&](unsigned int) {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_acq_rel);
            }
        };
        double time_ms = run_parallel(increment, NUM_THREADS);
        std::cout << "Acq-Rel:  " << time_ms << " ms" << std::endl;
    }
    
    // Sequential consistency
    {
        std::atomic<int64_t> counter{0};
        auto increment = [&](unsigned int) {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_seq_cst);
            }
        };
        double time_ms = run_parallel(increment, NUM_THREADS);
        std::cout << "Seq-Cst:  " << time_ms << " ms" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Note: On x86, relaxed and seq_cst often have similar performance" << std::endl;
    std::cout << "due to the strong memory model. ARM/POWER show bigger differences." << std::endl;
}

// ============================================================================
// Example 5: Compare-and-swap (CAS) operations
// ============================================================================

void demonstrate_cas_operations() {
    std::cout << std::endl << "=== Compare-and-Swap Operations ===" << std::endl;
    
    std::atomic<int> value{0};
    
    // Weak CAS - may fail spuriously, use in loops
    int expected = 0;
    bool success = value.compare_exchange_weak(expected, 1);
    std::cout << "Weak CAS (0 -> 1): " << (success ? "success" : "failed") << std::endl;
    
    // Strong CAS - won't fail spuriously
    expected = 1;
    success = value.compare_exchange_strong(expected, 2);
    std::cout << "Strong CAS (1 -> 2): " << (success ? "success" : "failed") << std::endl;
    
    // Failed CAS - expected is updated to current value
    expected = 0;  // Wrong expected value
    success = value.compare_exchange_strong(expected, 3);
    std::cout << "Failed CAS (expected 0, got " << expected << "): " 
              << (success ? "success" : "failed") << std::endl;
    
    std::cout << "Final value: " << value.load() << std::endl;
}

void demonstrate_atomic_ordering() {
    std::cout << "Hardware threads: " << hardware_concurrency() << std::endl;
    std::cout << std::endl;
    
    demonstrate_relaxed_ordering();
    demonstrate_acquire_release();
    demonstrate_sequential_consistency();
    benchmark_memory_orderings();
    demonstrate_cas_operations();
}

} // namespace hpc::concurrency

#ifndef HPC_BENCHMARK_MODE
int main() {
    hpc::concurrency::demonstrate_atomic_ordering();
    return 0;
}
#endif
