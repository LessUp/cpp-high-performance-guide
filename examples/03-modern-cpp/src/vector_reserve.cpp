/**
 * @file vector_reserve.cpp
 * @brief std::vector capacity management and reserve()
 *
 * This example demonstrates the importance of using reserve() to
 * pre-allocate vector capacity and avoid repeated reallocations.
 *
 * Key concepts:
 * - Vector growth strategy (typically 1.5x or 2x)
 * - reserve() vs resize()
 * - Counting allocations with custom allocator
 */

#include <chrono>
#include <hpc/vector_reserve.hpp>
#include <iostream>
#include <vector>

namespace hpc::vector_reserve {

using hpc::instrumentation::OperationMetrics;

//------------------------------------------------------------------------------
// Demonstrations
//------------------------------------------------------------------------------

void demonstrate_growth_pattern() {
    std::cout << "=== Vector Growth Pattern ===\n";

    std::vector<int> vec;
    size_t last_capacity = 0;

    std::cout << "Size\tCapacity\tReallocation\n";
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
        if (vec.capacity() != last_capacity) {
            std::cout << vec.size() << "\t" << vec.capacity() << "\t\tYes\n";
            last_capacity = vec.capacity();
        }
    }

    std::cout << "\nGrowth factor is typically 1.5x (MSVC) or 2x (GCC/Clang)\n";
}

void demonstrate_reserve_benefit() {
    std::cout << "\n=== Reserve vs No Reserve ===\n";

    constexpr size_t N = 1'000'000;

    // Without reserve
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        CountingAllocator<int> alloc(&metrics);
        std::vector<int, CountingAllocator<int>> vec(alloc);

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            vec.push_back(static_cast<int>(i));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "Without reserve:\n";
        std::cout << "  Time: " << ms << " ms\n";
        std::cout << "  Allocations: " << metrics.allocation_count << "\n";
        std::cout << "  Total bytes allocated: " << metrics.total_bytes_allocated / (1024 * 1024)
                  << " MB\n";
    }

    // With reserve
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        CountingAllocator<int> alloc(&metrics);
        std::vector<int, CountingAllocator<int>> vec(alloc);
        vec.reserve(N);  // Pre-allocate

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            vec.push_back(static_cast<int>(i));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "\nWith reserve(" << N << "):\n";
        std::cout << "  Time: " << ms << " ms\n";
        std::cout << "  Allocations: " << metrics.allocation_count << "\n";
        std::cout << "  Total bytes allocated: " << metrics.total_bytes_allocated / (1024 * 1024)
                  << " MB\n";
    }
}

void demonstrate_resize_vs_reserve() {
    std::cout << "\n=== resize() vs reserve() ===\n";

    // reserve() - allocates but doesn't initialize
    {
        std::vector<int> vec;
        vec.reserve(10);
        std::cout << "After reserve(10): size=" << vec.size() << ", capacity=" << vec.capacity()
                  << "\n";
        // vec[0] = 1;  // Undefined behavior! Size is still 0
    }

    // resize() - allocates AND initializes
    {
        std::vector<int> vec;
        vec.resize(10);
        std::cout << "After resize(10):  size=" << vec.size() << ", capacity=" << vec.capacity()
                  << "\n";
        vec[0] = 1;  // OK, size is 10
    }

    std::cout << "\nUse reserve() when you'll push_back elements\n";
    std::cout << "Use resize() when you need immediate indexed access\n";
}

void demonstrate_shrink_to_fit() {
    std::cout << "\n=== shrink_to_fit() ===\n";

    std::vector<int> vec;
    vec.reserve(1000);
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    std::cout << "After reserve(1000) and 10 push_backs:\n";
    std::cout << "  size=" << vec.size() << ", capacity=" << vec.capacity() << "\n";

    vec.shrink_to_fit();

    std::cout << "After shrink_to_fit():\n";
    std::cout << "  size=" << vec.size() << ", capacity=" << vec.capacity() << "\n";

    std::cout << "\nNote: shrink_to_fit() is a non-binding request\n";
}

void demonstrate_clear_vs_shrink() {
    std::cout << "\n=== clear() vs swap trick ===\n";

    std::vector<int> vec(1000);
    std::cout << "Initial: size=" << vec.size() << ", capacity=" << vec.capacity() << "\n";

    vec.clear();
    std::cout << "After clear(): size=" << vec.size() << ", capacity=" << vec.capacity() << "\n";

    // Swap trick to release memory
    std::vector<int>().swap(vec);
    std::cout << "After swap trick: size=" << vec.size() << ", capacity=" << vec.capacity() << "\n";
}

}  // namespace hpc::vector_reserve

#ifndef HPC_TEST_MODE
int main() {
    std::cout << "=== Vector Capacity Management Demo ===\n\n";

    hpc::vector_reserve::demonstrate_growth_pattern();
    hpc::vector_reserve::demonstrate_reserve_benefit();
    hpc::vector_reserve::demonstrate_resize_vs_reserve();
    hpc::vector_reserve::demonstrate_shrink_to_fit();
    hpc::vector_reserve::demonstrate_clear_vs_shrink();

    std::cout << "\nKey takeaways:\n";
    std::cout << "1. Always use reserve() when you know the final size\n";
    std::cout << "2. Without reserve(), vector may reallocate O(log N) times\n";
    std::cout << "3. Each reallocation copies all existing elements\n";
    std::cout << "4. Use shrink_to_fit() or swap trick to release excess capacity\n";

    return 0;
}
#endif
