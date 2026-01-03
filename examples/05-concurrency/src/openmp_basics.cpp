/**
 * @file openmp_basics.cpp
 * @brief OpenMP basics for simple parallelization
 * 
 * This example demonstrates:
 * 1. parallel for - Parallelizing loops
 * 2. reduction - Combining results from threads
 * 3. sections - Parallel independent tasks
 * 4. Thread scaling efficiency
 * 
 * Compile with: -fopenmp (GCC/Clang) or /openmp (MSVC)
 */

#include "../include/concurrency_utils.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <chrono>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace hpc::concurrency {

// ============================================================================
// Example 1: Parallel For
// ============================================================================

/**
 * Simple parallel for loop
 * Each iteration is independent, perfect for parallelization
 */
void parallel_for_example() {
    std::cout << "=== Parallel For Example ===" << std::endl;
    
    constexpr size_t N = 10000000;
    std::vector<double> data(N);
    
    // Sequential version
    auto seq_start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        data[i] = std::sin(static_cast<double>(i) * 0.001);
    }
    auto seq_end = std::chrono::high_resolution_clock::now();
    auto seq_time = std::chrono::duration<double, std::milli>(seq_end - seq_start).count();
    
    // Parallel version
    auto par_start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        data[i] = std::sin(static_cast<double>(i) * 0.001);
    }
    auto par_end = std::chrono::high_resolution_clock::now();
    auto par_time = std::chrono::duration<double, std::milli>(par_end - par_start).count();
    
    std::cout << "Array size: " << N << std::endl;
    std::cout << "Sequential time: " << seq_time << " ms" << std::endl;
    std::cout << "Parallel time: " << par_time << " ms" << std::endl;
    std::cout << "Speedup: " << seq_time / par_time << "x" << std::endl;
    std::cout << std::endl;
}

// ============================================================================
// Example 2: Reduction
// ============================================================================

/**
 * Parallel reduction - combining results from multiple threads
 * OpenMP handles the synchronization automatically
 */
void reduction_example() {
    std::cout << "=== Reduction Example ===" << std::endl;
    
    constexpr size_t N = 100000000;
    std::vector<double> data(N);
    
    // Initialize data
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        data[i] = 1.0 / (1.0 + static_cast<double>(i));
    }
    
    // Sequential sum
    auto seq_start = std::chrono::high_resolution_clock::now();
    double seq_sum = 0.0;
    for (size_t i = 0; i < N; ++i) {
        seq_sum += data[i];
    }
    auto seq_end = std::chrono::high_resolution_clock::now();
    auto seq_time = std::chrono::duration<double, std::milli>(seq_end - seq_start).count();
    
    // Parallel sum with reduction
    auto par_start = std::chrono::high_resolution_clock::now();
    double par_sum = 0.0;
    #pragma omp parallel for reduction(+:par_sum)
    for (size_t i = 0; i < N; ++i) {
        par_sum += data[i];
    }
    auto par_end = std::chrono::high_resolution_clock::now();
    auto par_time = std::chrono::duration<double, std::milli>(par_end - par_start).count();
    
    std::cout << "Array size: " << N << std::endl;
    std::cout << "Sequential sum: " << seq_sum << " (time: " << seq_time << " ms)" << std::endl;
    std::cout << "Parallel sum: " << par_sum << " (time: " << par_time << " ms)" << std::endl;
    std::cout << "Speedup: " << seq_time / par_time << "x" << std::endl;
    std::cout << "Results match: " << (std::abs(seq_sum - par_sum) < 1e-6 ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

// ============================================================================
// Example 3: Parallel Sections
// ============================================================================

/**
 * Parallel sections - running independent tasks in parallel
 */
void sections_example() {
    std::cout << "=== Parallel Sections Example ===" << std::endl;
    
    constexpr size_t N = 10000000;
    
    double result1 = 0.0, result2 = 0.0, result3 = 0.0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            // Task 1: Compute sum of squares
            for (size_t i = 0; i < N; ++i) {
                result1 += static_cast<double>(i * i);
            }
            #ifdef _OPENMP
            std::cout << "Section 1 completed by thread " << omp_get_thread_num() << std::endl;
            #endif
        }
        
        #pragma omp section
        {
            // Task 2: Compute sum of cubes
            for (size_t i = 0; i < N; ++i) {
                result2 += static_cast<double>(i * i * i);
            }
            #ifdef _OPENMP
            std::cout << "Section 2 completed by thread " << omp_get_thread_num() << std::endl;
            #endif
        }
        
        #pragma omp section
        {
            // Task 3: Compute harmonic series
            for (size_t i = 1; i <= N; ++i) {
                result3 += 1.0 / static_cast<double>(i);
            }
            #ifdef _OPENMP
            std::cout << "Section 3 completed by thread " << omp_get_thread_num() << std::endl;
            #endif
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "Results: " << result1 << ", " << result2 << ", " << result3 << std::endl;
    std::cout << "Total time: " << time << " ms" << std::endl;
    std::cout << std::endl;
}

// ============================================================================
// Example 4: Thread Scaling
// ============================================================================

/**
 * Measure scaling efficiency with different thread counts
 */
void scaling_example() {
    std::cout << "=== Thread Scaling Example ===" << std::endl;
    
    constexpr size_t N = 50000000;
    std::vector<double> data(N);
    
    // Initialize
    for (size_t i = 0; i < N; ++i) {
        data[i] = static_cast<double>(i);
    }
    
    // Measure with different thread counts
    #ifdef _OPENMP
    int max_threads = omp_get_max_threads();
    #else
    int max_threads = 1;
    #endif
    
    std::cout << "Max threads available: " << max_threads << std::endl;
    std::cout << std::endl;
    
    // Baseline: single thread
    double baseline_time = 0.0;
    {
        #ifdef _OPENMP
        omp_set_num_threads(1);
        #endif
        
        auto start = std::chrono::high_resolution_clock::now();
        double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
        for (size_t i = 0; i < N; ++i) {
            sum += std::sqrt(data[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        baseline_time = std::chrono::duration<double, std::milli>(end - start).count();
        
        std::cout << "Threads: 1, Time: " << baseline_time << " ms, Speedup: 1.00x, Efficiency: 100%" << std::endl;
    }
    
    // Test with increasing thread counts
    for (int threads = 2; threads <= max_threads; threads *= 2) {
        #ifdef _OPENMP
        omp_set_num_threads(threads);
        #endif
        
        auto start = std::chrono::high_resolution_clock::now();
        double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
        for (size_t i = 0; i < N; ++i) {
            sum += std::sqrt(data[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(end - start).count();
        
        double speedup = baseline_time / time;
        double efficiency = (speedup / threads) * 100.0;
        
        std::cout << "Threads: " << threads 
                  << ", Time: " << time << " ms"
                  << ", Speedup: " << speedup << "x"
                  << ", Efficiency: " << efficiency << "%" << std::endl;
    }
    
    // Reset to max threads
    #ifdef _OPENMP
    omp_set_num_threads(max_threads);
    #endif
    
    std::cout << std::endl;
}

// ============================================================================
// Example 5: Schedule Types
// ============================================================================

/**
 * Different scheduling strategies for load balancing
 */
void schedule_example() {
    std::cout << "=== Schedule Types Example ===" << std::endl;
    
    constexpr size_t N = 1000000;
    std::vector<double> data(N);
    
    // Work that varies by index (simulating uneven workload)
    auto work = [](size_t i) -> double {
        double result = 0.0;
        // More work for higher indices
        for (size_t j = 0; j < (i % 100) + 1; ++j) {
            result += std::sin(static_cast<double>(i + j));
        }
        return result;
    };
    
    // Static schedule (default)
    {
        auto start = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < N; ++i) {
            data[i] = work(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "Static schedule: " << time << " ms" << std::endl;
    }
    
    // Dynamic schedule (better for uneven workloads)
    {
        auto start = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for schedule(dynamic, 1000)
        for (size_t i = 0; i < N; ++i) {
            data[i] = work(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "Dynamic schedule (chunk=1000): " << time << " ms" << std::endl;
    }
    
    // Guided schedule (decreasing chunk sizes)
    {
        auto start = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for schedule(guided)
        for (size_t i = 0; i < N; ++i) {
            data[i] = work(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "Guided schedule: " << time << " ms" << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrate_openmp() {
    #ifdef _OPENMP
    std::cout << "OpenMP version: " << _OPENMP << std::endl;
    std::cout << "Max threads: " << omp_get_max_threads() << std::endl;
    #else
    std::cout << "OpenMP not available, running sequential versions" << std::endl;
    #endif
    std::cout << std::endl;
    
    parallel_for_example();
    reduction_example();
    sections_example();
    scaling_example();
    schedule_example();
}

} // namespace hpc::concurrency

#ifndef HPC_BENCHMARK_MODE
int main() {
    hpc::concurrency::demonstrate_openmp();
    return 0;
}
#endif
