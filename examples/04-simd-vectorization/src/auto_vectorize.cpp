/**
 * @file auto_vectorize.cpp
 * @brief Demonstrates code patterns that enable/disable automatic vectorization
 * 
 * This example shows:
 * 1. Code patterns that compilers can easily vectorize
 * 2. Anti-patterns that prevent vectorization
 * 3. How to check if code was vectorized (compiler reports)
 * 
 * Compile with: -O3 -march=native -fopt-info-vec (GCC) or -Rpass=loop-vectorize (Clang)
 */

#include "simd_utils.hpp"
#include <vector>
#include <cmath>
#include <numeric>
#include <iostream>

namespace hpc::simd {

// ============================================================================
// GOOD PATTERNS - Easy to vectorize
// ============================================================================

/**
 * Simple loop with contiguous memory access - VECTORIZABLE
 * 
 * This is the ideal pattern for auto-vectorization:
 * - Simple loop structure
 * - Contiguous memory access
 * - No data dependencies between iterations
 * - No function calls (or only inlined/vectorized functions)
 */
void add_arrays_vectorizable(const float* __restrict a, 
                              const float* __restrict b,
                              float* __restrict c, 
                              size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

/**
 * Scalar multiplication - VECTORIZABLE
 */
void scale_array_vectorizable(float* __restrict arr, float scalar, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

/**
 * Fused multiply-add - VECTORIZABLE
 * Modern CPUs have FMA instructions that can be auto-vectorized
 */
void fma_vectorizable(const float* __restrict a,
                      const float* __restrict b,
                      const float* __restrict c,
                      float* __restrict d,
                      size_t n) {
    for (size_t i = 0; i < n; ++i) {
        d[i] = a[i] * b[i] + c[i];
    }
}

/**
 * Reduction with OpenMP - VECTORIZABLE with proper hints
 */
float sum_array_vectorizable(const float* arr, size_t n) {
    float sum = 0.0f;
    #pragma omp simd reduction(+:sum)
    for (size_t i = 0; i < n; ++i) {
        sum += arr[i];
    }
    return sum;
}

/**
 * Conditional without branches - VECTORIZABLE
 * Using conditional moves instead of branches
 */
void clamp_array_vectorizable(float* __restrict arr, 
                               float min_val, 
                               float max_val, 
                               size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] = arr[i] < min_val ? min_val : (arr[i] > max_val ? max_val : arr[i]);
    }
}

// ============================================================================
// BAD PATTERNS - Difficult or impossible to vectorize
// ============================================================================

/**
 * Loop with data dependency - NOT VECTORIZABLE
 * Each iteration depends on the previous one
 */
void prefix_sum_not_vectorizable(float* arr, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        arr[i] += arr[i - 1];  // Dependency on previous iteration
    }
}

/**
 * Non-contiguous memory access - POORLY VECTORIZABLE
 * Strided access patterns hurt vectorization efficiency
 */
void strided_access_not_vectorizable(float* arr, size_t n, size_t stride) {
    for (size_t i = 0; i < n; i += stride) {
        arr[i] *= 2.0f;
    }
}

/**
 * Indirect indexing - NOT VECTORIZABLE
 * Gather/scatter operations are expensive
 */
void indirect_access_not_vectorizable(float* __restrict arr,
                                       const int* __restrict indices,
                                       size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[indices[i]] += 1.0f;  // Random access pattern
    }
}

/**
 * Function call in loop - MAY NOT VECTORIZE
 * Unless the function is inlined and vectorizable
 */
float external_function(float x);  // Declaration only - prevents inlining

void loop_with_call_not_vectorizable(float* arr, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] = external_function(arr[i]);
    }
}

/**
 * Complex control flow - NOT VECTORIZABLE
 * Multiple branches make vectorization difficult
 */
void complex_branches_not_vectorizable(float* arr, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (arr[i] > 0) {
            if (arr[i] > 10) {
                arr[i] = std::sqrt(arr[i]);
            } else {
                arr[i] = arr[i] * arr[i];
            }
        } else {
            arr[i] = -arr[i];
        }
    }
}

/**
 * Pointer aliasing - MAY NOT VECTORIZE
 * Without __restrict, compiler assumes pointers may alias
 */
void aliased_pointers_not_vectorizable(float* a, float* b, float* c, size_t n) {
    // Compiler cannot prove a, b, c don't overlap
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// ============================================================================
// IMPROVED PATTERNS - Fixing anti-patterns
// ============================================================================

/**
 * Parallel prefix sum - VECTORIZABLE with different algorithm
 * Using Blelloch scan algorithm (work-efficient parallel prefix)
 */
void parallel_prefix_sum(float* arr, size_t n) {
    if (n <= 1) return;
    
    // Up-sweep (reduce) phase
    for (size_t stride = 1; stride < n; stride *= 2) {
        #pragma omp simd
        for (size_t i = 2 * stride - 1; i < n; i += 2 * stride) {
            arr[i] += arr[i - stride];
        }
    }
    
    // Down-sweep phase
    arr[n - 1] = 0;
    for (size_t stride = n / 2; stride >= 1; stride /= 2) {
        #pragma omp simd
        for (size_t i = 2 * stride - 1; i < n; i += 2 * stride) {
            float temp = arr[i - stride];
            arr[i - stride] = arr[i];
            arr[i] += temp;
        }
    }
}

/**
 * Branchless version of complex control flow - VECTORIZABLE
 */
void branchless_vectorizable(float* arr, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        float x = arr[i];
        float abs_x = std::fabs(x);
        float sqrt_x = std::sqrt(abs_x);
        float sq_x = x * x;
        
        // Use conditional moves instead of branches
        float positive_result = (abs_x > 10.0f) ? sqrt_x : sq_x;
        arr[i] = (x > 0.0f) ? positive_result : abs_x;
    }
}

// ============================================================================
// Demo functions
// ============================================================================

void demonstrate_auto_vectorization() {
    constexpr size_t N = 1024 * 1024;
    
    std::cout << "=== Auto-Vectorization Demo ===" << std::endl;
    std::cout << "Detected SIMD level: " << simd_level_name(detect_simd_level()) << std::endl;
    std::cout << "Vector width: " << simd_vector_width(detect_simd_level()) << " bytes" << std::endl;
    std::cout << std::endl;
    
    // Allocate aligned memory
    AlignedBuffer<float> a(N), b(N), c(N);
    
    // Initialize
    for (size_t i = 0; i < N; ++i) {
        a[i] = static_cast<float>(i);
        b[i] = static_cast<float>(i) * 0.5f;
    }
    
    // Run vectorizable operations
    add_arrays_vectorizable(a.data(), b.data(), c.data(), N);
    scale_array_vectorizable(c.data(), 2.0f, N);
    
    float sum = sum_array_vectorizable(c.data(), N);
    std::cout << "Sum of scaled array: " << sum << std::endl;
    
    std::cout << std::endl;
    std::cout << "Vectorization tips:" << std::endl;
    std::cout << "1. Use -O3 -march=native for best auto-vectorization" << std::endl;
    std::cout << "2. Use __restrict to help compiler prove no aliasing" << std::endl;
    std::cout << "3. Avoid data dependencies between loop iterations" << std::endl;
    std::cout << "4. Use contiguous memory access patterns" << std::endl;
    std::cout << "5. Check vectorization with -fopt-info-vec (GCC) or -Rpass=loop-vectorize (Clang)" << std::endl;
}

} // namespace hpc::simd

// Provide definition for external_function to avoid linker errors
float hpc::simd::external_function(float x) {
    return x * x;
}

#ifndef HPC_BENCHMARK_MODE
int main() {
    hpc::simd::demonstrate_auto_vectorization();
    return 0;
}
#endif
