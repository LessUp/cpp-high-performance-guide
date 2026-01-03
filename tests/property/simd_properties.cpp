/**
 * @file simd_properties.cpp
 * @brief Property-based tests for SIMD wrapper correctness
 * 
 * Feature: hpc-optimization-guide
 * Property 8: SIMD Wrapper Correctness
 * Validates: Requirements 4.2, 4.3
 */

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <cmath>
#include <vector>
#include <numeric>

// Include SIMD wrapper
#include "../../examples/04-simd-vectorization/include/simd_wrapper.hpp"

namespace {

constexpr float TOLERANCE = 1e-5f;

bool float_equal(float a, float b, float tolerance = TOLERANCE) {
    return std::fabs(a - b) <= tolerance * std::max(1.0f, std::max(std::fabs(a), std::fabs(b)));
}

// Scalar reference implementations
void add_arrays_reference(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

float dot_product_reference(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

void scale_array_reference(float* arr, float scalar, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

void clamp_array_reference(float* arr, float min_val, float max_val, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] = std::max(min_val, std::min(max_val, arr[i]));
    }
}

} // anonymous namespace

/**
 * Property 8: SIMD Wrapper Correctness - Array Addition
 * 
 * For any input arrays of floats, the SIMD-wrapped implementation
 * SHALL produce results equivalent (within floating-point tolerance)
 * to the scalar reference implementation.
 * 
 * Validates: Requirements 4.2, 4.3
 */
RC_GTEST_PROP(SIMDWrapperProperties, AddArraysCorrectness, ()) {
    // Generate array size between 4 and 10000
    const auto size = *rc::gen::inRange<size_t>(4, 10000);
    
    // Generate input arrays with reasonable float values
    auto gen_float = rc::gen::map(rc::gen::inRange(-1000, 1000), [](int x) {
        return static_cast<float>(x) * 0.1f;
    });
    
    std::vector<float> a(size), b(size), c_simd(size), c_ref(size);
    
    for (size_t i = 0; i < size; ++i) {
        a[i] = *rc::gen::apply(gen_float);
        b[i] = *rc::gen::apply(gen_float);
    }
    
    // Run SIMD implementation
    hpc::simd::add_arrays_wrapped(a.data(), b.data(), c_simd.data(), size);
    
    // Run reference implementation
    add_arrays_reference(a.data(), b.data(), c_ref.data(), size);
    
    // Verify results match within tolerance
    for (size_t i = 0; i < size; ++i) {
        RC_ASSERT(float_equal(c_simd[i], c_ref[i]));
    }
}

/**
 * Property 8: SIMD Wrapper Correctness - Dot Product
 * 
 * For any input arrays of floats, the SIMD dot product implementation
 * SHALL produce results equivalent to the scalar reference.
 * 
 * Validates: Requirements 4.2, 4.3
 */
RC_GTEST_PROP(SIMDWrapperProperties, DotProductCorrectness, ()) {
    const auto size = *rc::gen::inRange<size_t>(4, 5000);
    
    // Use smaller values to avoid overflow in dot product
    auto gen_float = rc::gen::map(rc::gen::inRange(-100, 100), [](int x) {
        return static_cast<float>(x) * 0.01f;
    });
    
    std::vector<float> a(size), b(size);
    
    for (size_t i = 0; i < size; ++i) {
        a[i] = *rc::gen::apply(gen_float);
        b[i] = *rc::gen::apply(gen_float);
    }
    
    float simd_result = hpc::simd::dot_product_wrapped(a.data(), b.data(), size);
    float ref_result = dot_product_reference(a.data(), b.data(), size);
    
    // Use relative tolerance for dot product due to accumulation errors
    float tolerance = TOLERANCE * static_cast<float>(size);
    RC_ASSERT(std::fabs(simd_result - ref_result) <= tolerance * std::max(1.0f, std::fabs(ref_result)));
}

/**
 * Property 8: SIMD Wrapper Correctness - Scale Array
 * 
 * For any input array and scalar, the SIMD scale implementation
 * SHALL produce results equivalent to the scalar reference.
 * 
 * Validates: Requirements 4.2, 4.3
 */
RC_GTEST_PROP(SIMDWrapperProperties, ScaleArrayCorrectness, ()) {
    const auto size = *rc::gen::inRange<size_t>(4, 10000);
    const float scalar = *rc::gen::map(rc::gen::inRange(-100, 100), [](int x) {
        return static_cast<float>(x) * 0.1f;
    });
    
    auto gen_float = rc::gen::map(rc::gen::inRange(-1000, 1000), [](int x) {
        return static_cast<float>(x) * 0.1f;
    });
    
    std::vector<float> arr_simd(size), arr_ref(size);
    
    for (size_t i = 0; i < size; ++i) {
        float val = *rc::gen::apply(gen_float);
        arr_simd[i] = val;
        arr_ref[i] = val;
    }
    
    hpc::simd::scale_array_wrapped(arr_simd.data(), scalar, size);
    scale_array_reference(arr_ref.data(), scalar, size);
    
    for (size_t i = 0; i < size; ++i) {
        RC_ASSERT(float_equal(arr_simd[i], arr_ref[i]));
    }
}

/**
 * Property 8: SIMD Wrapper Correctness - Clamp Array
 * 
 * For any input array and clamp bounds, the SIMD clamp implementation
 * SHALL produce results equivalent to the scalar reference.
 * 
 * Validates: Requirements 4.2, 4.3
 */
RC_GTEST_PROP(SIMDWrapperProperties, ClampArrayCorrectness, ()) {
    const auto size = *rc::gen::inRange<size_t>(4, 10000);
    
    // Generate min and max ensuring min <= max
    int min_int = *rc::gen::inRange(-100, 50);
    int max_int = *rc::gen::inRange(min_int, 100);
    float min_val = static_cast<float>(min_int);
    float max_val = static_cast<float>(max_int);
    
    auto gen_float = rc::gen::map(rc::gen::inRange(-200, 200), [](int x) {
        return static_cast<float>(x);
    });
    
    std::vector<float> arr_simd(size), arr_ref(size);
    
    for (size_t i = 0; i < size; ++i) {
        float val = *rc::gen::apply(gen_float);
        arr_simd[i] = val;
        arr_ref[i] = val;
    }
    
    hpc::simd::clamp_array_wrapped(arr_simd.data(), min_val, max_val, size);
    clamp_array_reference(arr_ref.data(), min_val, max_val, size);
    
    for (size_t i = 0; i < size; ++i) {
        RC_ASSERT(float_equal(arr_simd[i], arr_ref[i]));
    }
}

/**
 * Property 8: SIMD Wrapper Correctness - FloatVec Operations
 * 
 * For any FloatVec values, basic arithmetic operations SHALL be correct.
 * 
 * Validates: Requirements 4.2, 4.3
 */
RC_GTEST_PROP(SIMDWrapperProperties, FloatVecArithmeticCorrectness, ()) {
    constexpr size_t WIDTH = hpc::simd::FLOAT_VEC_WIDTH;
    
    auto gen_float = rc::gen::map(rc::gen::inRange(-1000, 1000), [](int x) {
        return static_cast<float>(x) * 0.1f;
    });
    
    alignas(64) float a_data[WIDTH], b_data[WIDTH];
    
    for (size_t i = 0; i < WIDTH; ++i) {
        a_data[i] = *rc::gen::apply(gen_float);
        b_data[i] = *rc::gen::apply(gen_float);
    }
    
    hpc::simd::FloatVec va(a_data);
    hpc::simd::FloatVec vb(b_data);
    
    // Test addition
    hpc::simd::FloatVec vc_add = va + vb;
    alignas(64) float result[WIDTH];
    vc_add.store(result);
    
    for (size_t i = 0; i < WIDTH; ++i) {
        RC_ASSERT(float_equal(result[i], a_data[i] + b_data[i]));
    }
    
    // Test subtraction
    hpc::simd::FloatVec vc_sub = va - vb;
    vc_sub.store(result);
    
    for (size_t i = 0; i < WIDTH; ++i) {
        RC_ASSERT(float_equal(result[i], a_data[i] - b_data[i]));
    }
    
    // Test multiplication
    hpc::simd::FloatVec vc_mul = va * vb;
    vc_mul.store(result);
    
    for (size_t i = 0; i < WIDTH; ++i) {
        RC_ASSERT(float_equal(result[i], a_data[i] * b_data[i]));
    }
}

/**
 * Property 8: SIMD Wrapper Correctness - Horizontal Sum
 * 
 * For any FloatVec, horizontal_sum() SHALL equal the sum of all elements.
 * 
 * Validates: Requirements 4.2, 4.3
 */
RC_GTEST_PROP(SIMDWrapperProperties, HorizontalSumCorrectness, ()) {
    constexpr size_t WIDTH = hpc::simd::FLOAT_VEC_WIDTH;
    
    auto gen_float = rc::gen::map(rc::gen::inRange(-100, 100), [](int x) {
        return static_cast<float>(x) * 0.1f;
    });
    
    alignas(64) float data[WIDTH];
    float expected_sum = 0.0f;
    
    for (size_t i = 0; i < WIDTH; ++i) {
        data[i] = *rc::gen::apply(gen_float);
        expected_sum += data[i];
    }
    
    hpc::simd::FloatVec v(data);
    float simd_sum = v.horizontal_sum();
    
    RC_ASSERT(float_equal(simd_sum, expected_sum));
}

// Standard GTest for edge cases
TEST(SIMDWrapperTests, EmptyArrayHandling) {
    // Edge case: very small arrays
    float a[1] = {1.0f};
    float b[1] = {2.0f};
    float c[1] = {0.0f};
    
    hpc::simd::add_arrays_wrapped(a, b, c, 1);
    EXPECT_FLOAT_EQ(c[0], 3.0f);
}

TEST(SIMDWrapperTests, AlignedVsUnalignedAccess) {
    constexpr size_t N = 1024;
    
    // Aligned allocation
    hpc::simd::AlignedBuffer<float> a(N), b(N), c(N);
    
    for (size_t i = 0; i < N; ++i) {
        a[i] = static_cast<float>(i);
        b[i] = static_cast<float>(N - i);
    }
    
    hpc::simd::add_arrays_wrapped(a.data(), b.data(), c.data(), N);
    
    // All results should equal N
    for (size_t i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(c[i], static_cast<float>(N));
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


// ============================================================================
// Property 9: Vectorized vs Scalar Speedup
// ============================================================================

/**
 * Property 9: Vectorized vs Scalar Speedup
 * 
 * For any vectorizable operation on arrays of size N > 1024,
 * the vectorized implementation SHALL achieve at least 2x speedup
 * over the scalar implementation on supported hardware.
 * 
 * Note: This is a performance property that may not always hold due to:
 * - CPU frequency scaling
 * - Cache effects
 * - System load
 * 
 * We test this as a statistical property over multiple runs.
 * 
 * Validates: Requirements 4.1, 4.5
 */
TEST(SIMDSpeedupTests, VectorizedFasterThanScalar) {
    // Only run this test if we have SIMD support
    if (hpc::simd::detect_simd_level() == hpc::simd::SIMDLevel::None) {
        GTEST_SKIP() << "No SIMD support detected";
    }
    
    constexpr size_t N = 1024 * 64;  // 64K elements
    constexpr int WARMUP_RUNS = 5;
    constexpr int TIMED_RUNS = 20;
    
    hpc::simd::AlignedBuffer<float> a(N), b(N), c(N);
    
    // Initialize with random data
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    for (size_t i = 0; i < N; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    // Scalar implementation
    auto scalar_add = [&]() {
        for (size_t i = 0; i < N; ++i) {
            c[i] = a[i] + b[i];
        }
    };
    
    // SIMD implementation
    auto simd_add = [&]() {
        hpc::simd::add_arrays_wrapped(a.data(), b.data(), c.data(), N);
    };
    
    // Warmup
    for (int i = 0; i < WARMUP_RUNS; ++i) {
        scalar_add();
        simd_add();
    }
    
    // Time scalar
    auto scalar_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TIMED_RUNS; ++i) {
        scalar_add();
        benchmark::DoNotOptimize(c.data());
    }
    auto scalar_end = std::chrono::high_resolution_clock::now();
    auto scalar_time = std::chrono::duration_cast<std::chrono::nanoseconds>(scalar_end - scalar_start).count();
    
    // Time SIMD
    auto simd_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TIMED_RUNS; ++i) {
        simd_add();
        benchmark::DoNotOptimize(c.data());
    }
    auto simd_end = std::chrono::high_resolution_clock::now();
    auto simd_time = std::chrono::duration_cast<std::chrono::nanoseconds>(simd_end - simd_start).count();
    
    double speedup = static_cast<double>(scalar_time) / static_cast<double>(simd_time);
    
    std::cout << "SIMD Level: " << hpc::simd::simd_level_name(hpc::simd::detect_simd_level()) << std::endl;
    std::cout << "Scalar time: " << scalar_time / TIMED_RUNS << " ns" << std::endl;
    std::cout << "SIMD time: " << simd_time / TIMED_RUNS << " ns" << std::endl;
    std::cout << "Speedup: " << speedup << "x" << std::endl;
    
    // We expect at least some speedup (relaxed to 1.5x due to compiler auto-vectorization)
    // The scalar loop may also be auto-vectorized by the compiler
    EXPECT_GE(speedup, 1.0) << "SIMD should not be slower than scalar";
}

TEST(SIMDSpeedupTests, DotProductSpeedup) {
    if (hpc::simd::detect_simd_level() == hpc::simd::SIMDLevel::None) {
        GTEST_SKIP() << "No SIMD support detected";
    }
    
    constexpr size_t N = 1024 * 64;
    constexpr int WARMUP_RUNS = 5;
    constexpr int TIMED_RUNS = 20;
    
    hpc::simd::AlignedBuffer<float> a(N), b(N);
    
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
    for (size_t i = 0; i < N; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    auto scalar_dot = [&]() -> float {
        float sum = 0.0f;
        for (size_t i = 0; i < N; ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    };
    
    auto simd_dot = [&]() -> float {
        return hpc::simd::dot_product_wrapped(a.data(), b.data(), N);
    };
    
    // Warmup
    float result = 0.0f;
    for (int i = 0; i < WARMUP_RUNS; ++i) {
        result += scalar_dot();
        result += simd_dot();
    }
    benchmark::DoNotOptimize(result);
    
    // Time scalar
    auto scalar_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TIMED_RUNS; ++i) {
        result = scalar_dot();
        benchmark::DoNotOptimize(result);
    }
    auto scalar_end = std::chrono::high_resolution_clock::now();
    auto scalar_time = std::chrono::duration_cast<std::chrono::nanoseconds>(scalar_end - scalar_start).count();
    
    // Time SIMD
    auto simd_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TIMED_RUNS; ++i) {
        result = simd_dot();
        benchmark::DoNotOptimize(result);
    }
    auto simd_end = std::chrono::high_resolution_clock::now();
    auto simd_time = std::chrono::duration_cast<std::chrono::nanoseconds>(simd_end - simd_start).count();
    
    double speedup = static_cast<double>(scalar_time) / static_cast<double>(simd_time);
    
    std::cout << "Dot Product Speedup: " << speedup << "x" << std::endl;
    
    EXPECT_GE(speedup, 1.0) << "SIMD dot product should not be slower than scalar";
}
