/**
 * @file false_sharing.cpp
 * @brief Demonstrates false sharing in multi-threaded code
 * 
 * False sharing occurs when threads on different cores modify variables
 * that reside on the same cache line. Even though the variables are
 * logically independent, the cache coherency protocol causes the cache
 * line to bounce between cores, severely degrading performance.
 * 
 * Key concepts:
 * - Cache line size (typically 64 bytes on x86)
 * - Cache coherency protocols (MESI)
 * - alignas() for cache line alignment
 */

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace hpc::memory {

constexpr size_t CACHE_LINE_SIZE = 64;

//------------------------------------------------------------------------------
// Bad: Counters packed together (false sharing)
//------------------------------------------------------------------------------

/**
 * @brief Counters that will suffer from false sharing
 * 
 * When multiple threads increment different counters, they will
 * invalidate each other's cache lines because the counters are
 * adjacent in memory.
 */
struct PackedCounters {
    std::atomic<int64_t> counter1{0};
    std::atomic<int64_t> counter2{0};
    std::atomic<int64_t> counter3{0};
    std::atomic<int64_t> counter4{0};
};

//------------------------------------------------------------------------------
// Good: Counters padded to separate cache lines
//------------------------------------------------------------------------------

/**
 * @brief Counter padded to its own cache line
 * 
 * Using alignas(64) ensures each counter occupies its own cache line,
 * preventing false sharing between threads.
 */
struct alignas(CACHE_LINE_SIZE) PaddedCounter {
    std::atomic<int64_t> value{0};
    // Padding is implicit due to alignas
};

struct PaddedCounters {
    PaddedCounter counter1;
    PaddedCounter counter2;
    PaddedCounter counter3;
    PaddedCounter counter4;
};

//------------------------------------------------------------------------------
// Benchmark functions
//------------------------------------------------------------------------------

void increment_packed(PackedCounters& counters, int thread_id, int64_t iterations) {
    std::atomic<int64_t>* counter = nullptr;
    switch (thread_id % 4) {
        case 0: counter = &counters.counter1; break;
        case 1: counter = &counters.counter2; break;
        case 2: counter = &counters.counter3; break;
        case 3: counter = &counters.counter4; break;
    }
    
    for (int64_t i = 0; i < iterations; ++i) {
        counter->fetch_add(1, std::memory_order_relaxed);
    }
}

void increment_padded(PaddedCounters& counters, int thread_id, int64_t iterations) {
    PaddedCounter* counter = nullptr;
    switch (thread_id % 4) {
        case 0: counter = &counters.counter1; break;
        case 1: counter = &counters.counter2; break;
        case 2: counter = &counters.counter3; break;
        case 3: counter = &counters.counter4; break;
    }
    
    for (int64_t i = 0; i < iterations; ++i) {
        counter->value.fetch_add(1, std::memory_order_relaxed);
    }
}

void run_packed_benchmark(int num_threads, int64_t iterations_per_thread) {
    PackedCounters counters;
    std::vector<std::thread> threads;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(increment_packed, std::ref(counters), t, iterations_per_thread);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    int64_t total = counters.counter1.load() + counters.counter2.load() +
                    counters.counter3.load() + counters.counter4.load();
    
    std::cout << "Packed (false sharing):  " << duration.count() << " ms"
              << " (total: " << total << ")\n";
}

void run_padded_benchmark(int num_threads, int64_t iterations_per_thread) {
    PaddedCounters counters;
    std::vector<std::thread> threads;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(increment_padded, std::ref(counters), t, iterations_per_thread);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    int64_t total = counters.counter1.value.load() + counters.counter2.value.load() +
                    counters.counter3.value.load() + counters.counter4.value.load();
    
    std::cout << "Padded (no false sharing): " << duration.count() << " ms"
              << " (total: " << total << ")\n";
}

} // namespace hpc::memory

int main() {
    using namespace hpc::memory;
    
    std::cout << "=== False Sharing Demonstration ===\n\n";
    
    std::cout << "Cache line size: " << CACHE_LINE_SIZE << " bytes\n";
    std::cout << "sizeof(PackedCounters): " << sizeof(PackedCounters) << " bytes\n";
    std::cout << "sizeof(PaddedCounters): " << sizeof(PaddedCounters) << " bytes\n\n";
    
    const int num_threads = 4;
    const int64_t iterations = 10'000'000;
    
    std::cout << "Threads: " << num_threads << "\n";
    std::cout << "Iterations per thread: " << iterations << "\n\n";
    
    // Run benchmarks
    run_packed_benchmark(num_threads, iterations);
    run_padded_benchmark(num_threads, iterations);
    
    std::cout << "\nNote: Padded counters should be significantly faster because\n";
    std::cout << "each thread's counter is on its own cache line, avoiding\n";
    std::cout << "cache line bouncing between cores.\n";
    
    return 0;
}
