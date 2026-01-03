/**
 * @file ranges_vs_loops.cpp
 * @brief C++20 Ranges vs Raw Loops performance comparison
 * 
 * This example compares the performance of C++20 ranges with
 * traditional raw loops to analyze any overhead.
 * 
 * Key concepts:
 * - std::ranges algorithms
 * - Range views (lazy evaluation)
 * - Compiler optimization of ranges
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <ranges>
#include <vector>

namespace hpc::ranges {

//------------------------------------------------------------------------------
// Transform operations
//------------------------------------------------------------------------------

/**
 * @brief Transform using raw loop
 */
void transform_raw_loop(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] * 2 + 1;
    }
}

/**
 * @brief Transform using std::transform
 */
void transform_algorithm(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    std::transform(input.begin(), input.end(), output.begin(),
                   [](int x) { return x * 2 + 1; });
}

/**
 * @brief Transform using ranges
 */
void transform_ranges(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    std::ranges::transform(input, output.begin(),
                           [](int x) { return x * 2 + 1; });
}

//------------------------------------------------------------------------------
// Filter operations
//------------------------------------------------------------------------------

/**
 * @brief Filter using raw loop
 */
std::vector<int> filter_raw_loop(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);  // Estimate
    for (int x : input) {
        if (x % 2 == 0) {
            output.push_back(x);
        }
    }
    return output;
}

/**
 * @brief Filter using std::copy_if
 */
std::vector<int> filter_algorithm(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);
    std::copy_if(input.begin(), input.end(), std::back_inserter(output),
                 [](int x) { return x % 2 == 0; });
    return output;
}

/**
 * @brief Filter using ranges view (lazy)
 */
auto filter_ranges_view(const std::vector<int>& input) {
    return input | std::views::filter([](int x) { return x % 2 == 0; });
}

//------------------------------------------------------------------------------
// Chained operations
//------------------------------------------------------------------------------

/**
 * @brief Filter then transform using raw loops
 */
std::vector<int> chain_raw_loop(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);
    for (int x : input) {
        if (x % 2 == 0) {
            output.push_back(x * 2 + 1);
        }
    }
    return output;
}

/**
 * @brief Filter then transform using ranges (lazy, single pass)
 */
auto chain_ranges_view(const std::vector<int>& input) {
    return input 
        | std::views::filter([](int x) { return x % 2 == 0; })
        | std::views::transform([](int x) { return x * 2 + 1; });
}

/**
 * @brief Materialize a range view into a vector
 */
template<std::ranges::range R>
std::vector<std::ranges::range_value_t<R>> to_vector(R&& range) {
    std::vector<std::ranges::range_value_t<R>> result;
    for (auto&& elem : range) {
        result.push_back(std::forward<decltype(elem)>(elem));
    }
    return result;
}

//------------------------------------------------------------------------------
// Sum operations
//------------------------------------------------------------------------------

/**
 * @brief Sum using raw loop
 */
int64_t sum_raw_loop(const std::vector<int>& input) {
    int64_t sum = 0;
    for (int x : input) {
        sum += x;
    }
    return sum;
}

/**
 * @brief Sum using std::accumulate
 */
int64_t sum_algorithm(const std::vector<int>& input) {
    return std::accumulate(input.begin(), input.end(), int64_t{0});
}

/**
 * @brief Sum using ranges fold
 */
int64_t sum_ranges(const std::vector<int>& input) {
    // C++23 has std::ranges::fold_left, for C++20 we use a workaround
    int64_t sum = 0;
    for (int x : input) {
        sum += x;
    }
    return sum;
}

//------------------------------------------------------------------------------
// Benchmarks
//------------------------------------------------------------------------------

void benchmark_transform() {
    std::cout << "=== Transform Benchmark ===\n";
    
    constexpr size_t N = 10'000'000;
    constexpr int ITERATIONS = 10;
    
    std::vector<int> input(N);
    std::iota(input.begin(), input.end(), 0);
    std::vector<int> output;
    
    // Raw loop
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            transform_raw_loop(input, output);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Raw loop:      " << ms << " ms\n";
    }
    
    // std::transform
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            transform_algorithm(input, output);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "std::transform: " << ms << " ms\n";
    }
    
    // ranges::transform
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            transform_ranges(input, output);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "ranges::transform: " << ms << " ms\n";
    }
}

void benchmark_filter() {
    std::cout << "\n=== Filter Benchmark ===\n";
    
    constexpr size_t N = 10'000'000;
    constexpr int ITERATIONS = 10;
    
    std::vector<int> input(N);
    std::iota(input.begin(), input.end(), 0);
    
    // Raw loop
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            auto result = filter_raw_loop(input);
            volatile size_t s = result.size();
            (void)s;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Raw loop:    " << ms << " ms\n";
    }
    
    // std::copy_if
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            auto result = filter_algorithm(input);
            volatile size_t s = result.size();
            (void)s;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "std::copy_if: " << ms << " ms\n";
    }
    
    // Ranges view (lazy, just iteration)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            auto view = filter_ranges_view(input);
            int64_t sum = 0;
            for (int x : view) {
                sum += x;
            }
            volatile int64_t s = sum;
            (void)s;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Ranges view (lazy sum): " << ms << " ms\n";
    }
}

void benchmark_chain() {
    std::cout << "\n=== Chained Operations Benchmark ===\n";
    
    constexpr size_t N = 10'000'000;
    constexpr int ITERATIONS = 10;
    
    std::vector<int> input(N);
    std::iota(input.begin(), input.end(), 0);
    
    // Raw loop
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            auto result = chain_raw_loop(input);
            volatile size_t s = result.size();
            (void)s;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Raw loop:    " << ms << " ms\n";
    }
    
    // Ranges (lazy, materialized)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            auto view = chain_ranges_view(input);
            auto result = to_vector(view);
            volatile size_t s = result.size();
            (void)s;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Ranges (materialized): " << ms << " ms\n";
    }
    
    // Ranges (lazy, just sum)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            auto view = chain_ranges_view(input);
            int64_t sum = 0;
            for (int x : view) {
                sum += x;
            }
            volatile int64_t s = sum;
            (void)s;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Ranges (lazy sum): " << ms << " ms\n";
    }
}

} // namespace hpc::ranges

int main() {
    std::cout << "=== C++20 Ranges vs Raw Loops ===\n\n";
    
    hpc::ranges::benchmark_transform();
    hpc::ranges::benchmark_filter();
    hpc::ranges::benchmark_chain();
    
    std::cout << "\nKey observations:\n";
    std::cout << "1. For simple operations, performance is usually equivalent\n";
    std::cout << "2. Ranges views are lazy - no intermediate allocations\n";
    std::cout << "3. Chained operations with views can be more efficient\n";
    std::cout << "4. Modern compilers optimize ranges well\n";
    
    return 0;
}
