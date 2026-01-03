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
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

namespace hpc::vector_reserve {

//------------------------------------------------------------------------------
// Counting allocator to track allocations
//------------------------------------------------------------------------------

template<typename T>
class CountingAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    CountingAllocator() noexcept = default;
    
    template<typename U>
    CountingAllocator(const CountingAllocator<U>&) noexcept {}
    
    T* allocate(std::size_t n) {
        ++allocation_count_;
        total_bytes_allocated_ += n * sizeof(T);
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }
    
    void deallocate(T* ptr, std::size_t n) noexcept {
        ++deallocation_count_;
        total_bytes_deallocated_ += n * sizeof(T);
        std::free(ptr);
    }
    
    static void reset_counts() {
        allocation_count_ = 0;
        deallocation_count_ = 0;
        total_bytes_allocated_ = 0;
        total_bytes_deallocated_ = 0;
    }
    
    static size_t allocation_count_;
    static size_t deallocation_count_;
    static size_t total_bytes_allocated_;
    static size_t total_bytes_deallocated_;
};

template<typename T>
size_t CountingAllocator<T>::allocation_count_ = 0;

template<typename T>
size_t CountingAllocator<T>::deallocation_count_ = 0;

template<typename T>
size_t CountingAllocator<T>::total_bytes_allocated_ = 0;

template<typename T>
size_t CountingAllocator<T>::total_bytes_deallocated_ = 0;

template<typename T, typename U>
bool operator==(const CountingAllocator<T>&, const CountingAllocator<U>&) noexcept {
    return true;
}

template<typename T, typename U>
bool operator!=(const CountingAllocator<T>&, const CountingAllocator<U>&) noexcept {
    return false;
}

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
    
    using CountingVector = std::vector<int, CountingAllocator<int>>;
    
    // Without reserve
    {
        CountingAllocator<int>::reset_counts();
        
        auto start = std::chrono::high_resolution_clock::now();
        CountingVector vec;
        for (size_t i = 0; i < N; ++i) {
            vec.push_back(static_cast<int>(i));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "Without reserve:\n";
        std::cout << "  Time: " << ms << " ms\n";
        std::cout << "  Allocations: " << CountingAllocator<int>::allocation_count_ << "\n";
        std::cout << "  Total bytes allocated: " 
                  << CountingAllocator<int>::total_bytes_allocated_ / (1024 * 1024) << " MB\n";
    }
    
    // With reserve
    {
        CountingAllocator<int>::reset_counts();
        
        auto start = std::chrono::high_resolution_clock::now();
        CountingVector vec;
        vec.reserve(N);  // Pre-allocate
        for (size_t i = 0; i < N; ++i) {
            vec.push_back(static_cast<int>(i));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "\nWith reserve(" << N << "):\n";
        std::cout << "  Time: " << ms << " ms\n";
        std::cout << "  Allocations: " << CountingAllocator<int>::allocation_count_ << "\n";
        std::cout << "  Total bytes allocated: " 
                  << CountingAllocator<int>::total_bytes_allocated_ / (1024 * 1024) << " MB\n";
    }
}

void demonstrate_resize_vs_reserve() {
    std::cout << "\n=== resize() vs reserve() ===\n";
    
    // reserve() - allocates but doesn't initialize
    {
        std::vector<int> vec;
        vec.reserve(10);
        std::cout << "After reserve(10): size=" << vec.size() 
                  << ", capacity=" << vec.capacity() << "\n";
        // vec[0] = 1;  // Undefined behavior! Size is still 0
    }
    
    // resize() - allocates AND initializes
    {
        std::vector<int> vec;
        vec.resize(10);
        std::cout << "After resize(10):  size=" << vec.size() 
                  << ", capacity=" << vec.capacity() << "\n";
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

} // namespace hpc::vector_reserve

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
