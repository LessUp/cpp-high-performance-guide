/**
 * @file prefetch.cpp
 * @brief Software prefetching demonstration
 * 
 * This example shows how to use __builtin_prefetch to hint the CPU
 * to load data into cache before it's needed. This can improve
 * performance for predictable access patterns on large arrays.
 * 
 * Key concepts:
 * - Hardware vs software prefetching
 * - Prefetch distance tuning
 * - When prefetching helps (and when it doesn't)
 */

#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

namespace hpc::memory {

//------------------------------------------------------------------------------
// Prefetch intrinsics
//------------------------------------------------------------------------------

/**
 * @brief Prefetch for read with high temporal locality
 */
template<typename T>
inline void prefetch_read(const T* ptr) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ptr, 0, 3);  // Read, high temporal locality
#elif defined(_MSC_VER)
    _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0);
#endif
}

/**
 * @brief Prefetch for write
 */
template<typename T>
inline void prefetch_write(T* ptr) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ptr, 1, 3);  // Write, high temporal locality
#endif
}

//------------------------------------------------------------------------------
// Array traversal implementations
//------------------------------------------------------------------------------

/**
 * @brief Simple sequential sum without prefetching
 */
int64_t sum_no_prefetch(const int64_t* data, size_t n) {
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += data[i];
    }
    return sum;
}

/**
 * @brief Sequential sum with software prefetching
 * 
 * Prefetch distance is tuned for typical cache latency.
 * Too small: data not ready when needed
 * Too large: data evicted before use
 */
int64_t sum_with_prefetch(const int64_t* data, size_t n) {
    constexpr size_t PREFETCH_DISTANCE = 16;  // Prefetch 16 elements ahead
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // Prefetch future data
        if (i + PREFETCH_DISTANCE < n) {
            prefetch_read(&data[i + PREFETCH_DISTANCE]);
        }
        sum += data[i];
    }
    return sum;
}

/**
 * @brief Random access sum without prefetching
 * 
 * Random access patterns are harder to optimize because
 * the CPU can't predict what to prefetch.
 */
int64_t sum_random_no_prefetch(const int64_t* data, const size_t* indices, size_t n) {
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += data[indices[i]];
    }
    return sum;
}

/**
 * @brief Random access sum with software prefetching
 * 
 * For random access, we can prefetch the next few indices
 * to hide memory latency.
 */
int64_t sum_random_with_prefetch(const int64_t* data, const size_t* indices, size_t n) {
    constexpr size_t PREFETCH_DISTANCE = 8;
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // Prefetch data for future iterations
        if (i + PREFETCH_DISTANCE < n) {
            prefetch_read(&data[indices[i + PREFETCH_DISTANCE]]);
        }
        sum += data[indices[i]];
    }
    return sum;
}

//------------------------------------------------------------------------------
// Linked list traversal (pointer chasing)
//------------------------------------------------------------------------------

struct Node {
    int64_t value;
    Node* next;
};

/**
 * @brief Linked list sum without prefetching
 */
int64_t sum_list_no_prefetch(const Node* head) {
    int64_t sum = 0;
    for (const Node* node = head; node != nullptr; node = node->next) {
        sum += node->value;
    }
    return sum;
}

/**
 * @brief Linked list sum with prefetching
 * 
 * Prefetch the next node while processing current node.
 */
int64_t sum_list_with_prefetch(const Node* head) {
    int64_t sum = 0;
    for (const Node* node = head; node != nullptr; node = node->next) {
        // Prefetch next node
        if (node->next != nullptr) {
            prefetch_read(node->next);
        }
        sum += node->value;
    }
    return sum;
}

//------------------------------------------------------------------------------
// Benchmark
//------------------------------------------------------------------------------

void run_benchmark() {
    constexpr size_t N = 100'000'000;
    constexpr int ITERATIONS = 5;
    
    std::cout << "Array size: " << N << " elements (" 
              << (N * sizeof(int64_t) / (1024 * 1024)) << " MB)\n";
    std::cout << "Iterations: " << ITERATIONS << "\n\n";
    
    // Initialize data
    std::vector<int64_t> data(N);
    for (size_t i = 0; i < N; ++i) {
        data[i] = static_cast<int64_t>(i % 1000);
    }
    
    // Sequential access benchmarks
    std::cout << "=== Sequential Access ===\n";
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += sum_no_prefetch(data.data(), N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "No prefetch:   " << ms << " ms (sum: " << sum << ")\n";
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += sum_with_prefetch(data.data(), N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "With prefetch: " << ms << " ms (sum: " << sum << ")\n";
    }
    
    // Random access benchmarks
    std::cout << "\n=== Random Access ===\n";
    
    std::vector<size_t> indices(N);
    for (size_t i = 0; i < N; ++i) {
        indices[i] = i;
    }
    
    // Shuffle indices
    std::mt19937 rng(42);
    for (size_t i = N - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        std::swap(indices[i], indices[dist(rng)]);
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += sum_random_no_prefetch(data.data(), indices.data(), N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "No prefetch:   " << ms << " ms (sum: " << sum << ")\n";
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += sum_random_with_prefetch(data.data(), indices.data(), N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "With prefetch: " << ms << " ms (sum: " << sum << ")\n";
    }
}

} // namespace hpc::memory

int main() {
    std::cout << "=== Software Prefetching Demonstration ===\n\n";
    hpc::memory::run_benchmark();
    
    std::cout << "\nNotes:\n";
    std::cout << "- Sequential access: Hardware prefetcher is very effective,\n";
    std::cout << "  so software prefetching may not help much.\n";
    std::cout << "- Random access: Software prefetching can help by hiding\n";
    std::cout << "  memory latency when access pattern is known ahead of time.\n";
    
    return 0;
}
