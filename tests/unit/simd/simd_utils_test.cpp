/**
 * @file simd_utils_test.cpp
 * @brief Unit tests for simd_utils.hpp and simd_wrapper.hpp
 */

#include "simd_utils.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numeric>

#include "simd_wrapper.hpp"

namespace hpc::simd::test {

// ---------------------------------------------------------------------------
// Alignment utilities
// ---------------------------------------------------------------------------

TEST(SimdUtilsTest, IsAligned) {
    alignas(64) float data[16];
    EXPECT_TRUE(is_aligned(data, 16));
    EXPECT_TRUE(is_aligned(data, 32));
    EXPECT_TRUE(is_aligned(data, 64));
}

TEST(SimdUtilsTest, AlignUp) {
    EXPECT_EQ(align_up(1, 16), 16u);
    EXPECT_EQ(align_up(16, 16), 16u);
    EXPECT_EQ(align_up(17, 16), 32u);
    EXPECT_EQ(align_up(0, 32), 0u);
    EXPECT_EQ(align_up(33, 32), 64u);
}

TEST(SimdUtilsTest, GetSimdAlignment) {
    size_t alignment = get_simd_alignment();
    EXPECT_GE(alignment, sizeof(void*));
    // Must be a power of 2
    EXPECT_EQ(alignment & (alignment - 1), 0u);
}

// ---------------------------------------------------------------------------
// Aligned allocator
// ---------------------------------------------------------------------------

TEST(AlignedAllocatorTest, VectorAllocates) {
    aligned_vector<float> vec(1024, 1.0f);
    EXPECT_EQ(vec.size(), 1024u);
    EXPECT_TRUE(is_aligned(vec.data(), get_simd_alignment()));
}

// ---------------------------------------------------------------------------
// SIMD level detection
// ---------------------------------------------------------------------------

TEST(SimdLevelTest, DetectedLevelIsValid) {
    SIMDLevel level = detect_simd_level();
    EXPECT_NE(simd_level_name(level), nullptr);
    EXPECT_GT(simd_vector_width(level), 0u);
}

// ---------------------------------------------------------------------------
// SIMD wrapper: add_arrays_wrapped
// ---------------------------------------------------------------------------

TEST(SimdWrapperTest, AddArrays) {
    constexpr size_t N = 1024;
    aligned_vector<float> a(N), b(N), c(N);

    for (size_t i = 0; i < N; ++i) {
        a[i] = static_cast<float>(i);
        b[i] = static_cast<float>(N - i);
    }

    add_arrays_wrapped(a.data(), b.data(), c.data(), N);

    for (size_t i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(c[i], static_cast<float>(N));
    }
}

// ---------------------------------------------------------------------------
// SIMD wrapper: dot_product_wrapped
// ---------------------------------------------------------------------------

TEST(SimdWrapperTest, DotProduct) {
    constexpr size_t N = 256;
    aligned_vector<float> a(N, 1.0f);
    aligned_vector<float> b(N, 2.0f);

    float result = dot_product_wrapped(a.data(), b.data(), N);
    EXPECT_NEAR(result, static_cast<float>(N * 2), 0.01f);
}

// ---------------------------------------------------------------------------
// SIMD wrapper: scale_array_wrapped
// ---------------------------------------------------------------------------

TEST(SimdWrapperTest, ScaleArray) {
    constexpr size_t N = 512;
    aligned_vector<float> arr(N, 3.0f);

    scale_array_wrapped(arr.data(), 2.0f, N);

    for (size_t i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(arr[i], 6.0f);
    }
}

// ---------------------------------------------------------------------------
// SIMD wrapper: clamp_array_wrapped
// ---------------------------------------------------------------------------

TEST(SimdWrapperTest, ClampArray) {
    constexpr size_t N = 128;
    aligned_vector<float> arr(N);

    for (size_t i = 0; i < N; ++i) {
        arr[i] = static_cast<float>(i) - 64.0f;  // range [-64, 63]
    }

    clamp_array_wrapped(arr.data(), -10.0f, 10.0f, N);

    for (size_t i = 0; i < N; ++i) {
        EXPECT_GE(arr[i], -10.0f);
        EXPECT_LE(arr[i], 10.0f);
    }
}

// ---------------------------------------------------------------------------
// Edge case: non-multiple-of-vector-width sizes
// ---------------------------------------------------------------------------

TEST(SimdWrapperTest, NonAlignedSize) {
    constexpr size_t N = 17;  // Not a multiple of any SIMD width
    aligned_vector<float> a(N, 1.0f);
    aligned_vector<float> b(N, 2.0f);
    aligned_vector<float> c(N);

    add_arrays_wrapped(a.data(), b.data(), c.data(), N);

    for (size_t i = 0; i < N; ++i) {
        EXPECT_FLOAT_EQ(c[i], 3.0f);
    }
}

}  // namespace hpc::simd::test
