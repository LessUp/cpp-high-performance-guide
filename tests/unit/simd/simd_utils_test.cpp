/**
 * @file simd_utils_test.cpp
 * @brief Unit tests for the canonical hpc::simd module
 */

#include <gtest/gtest.h>

#include <cmath>
#include <hpc/simd.hpp>
#include <numeric>

namespace hpc::simd::test {

// ---------------------------------------------------------------------------
// Alignment utilities
// ---------------------------------------------------------------------------

TEST(SimdUtilsTest, IsAligned) {
    alignas(64) float data[16]{};
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

// ---------------------------------------------------------------------------
// make_aligned_vector factory
// ---------------------------------------------------------------------------

TEST(MakeAlignedVectorTest, DefaultConstructedIsSizedAndAligned) {
    auto vec = make_aligned_vector<float>(1000);
    EXPECT_EQ(vec.size(), 1000u);
    ASSERT_NE(vec.data(), nullptr);
    EXPECT_TRUE(is_aligned(vec.data(), get_simd_alignment()));
}

TEST(MakeAlignedVectorTest, ValueConstructorFillsAndAligns) {
    auto vec = make_aligned_vector<float>(256, 3.5f);
    EXPECT_EQ(vec.size(), 256u);
    ASSERT_NE(vec.data(), nullptr);
    EXPECT_TRUE(is_aligned(vec.data(), get_simd_alignment()));
    for (float value : vec) {
        EXPECT_FLOAT_EQ(value, 3.5f);
    }
}

// ---------------------------------------------------------------------------
// SimdVec (FloatVec): sqrt and lane-wise division
// ---------------------------------------------------------------------------

TEST(SimdVecTest, SqrtOfPerfectSquares) {
    alignas(64) float in[FLOAT_VEC_WIDTH];
    for (size_t i = 0; i < FLOAT_VEC_WIDTH; ++i) {
        const float base = static_cast<float>(i + 1);
        in[i] = base * base;
    }

    const FloatVec result = FloatVec(in).sqrt();

    alignas(64) float out[FLOAT_VEC_WIDTH];
    result.store(out);
    for (size_t i = 0; i < FLOAT_VEC_WIDTH; ++i) {
        EXPECT_FLOAT_EQ(out[i], static_cast<float>(i + 1));
    }
}

TEST(SimdVecTest, DivisionIsLaneWise) {
    alignas(64) float num[FLOAT_VEC_WIDTH];
    alignas(64) float den[FLOAT_VEC_WIDTH];
    for (size_t i = 0; i < FLOAT_VEC_WIDTH; ++i) {
        num[i] = static_cast<float>(i + 1) * 6.0f;
        den[i] = static_cast<float>(i + 1);
    }

    const FloatVec broadcast_result = FloatVec(8.0f) / FloatVec(2.0f);
    const FloatVec varying_result = FloatVec(num) / FloatVec(den);

    alignas(64) float broadcast_out[FLOAT_VEC_WIDTH];
    alignas(64) float varying_out[FLOAT_VEC_WIDTH];
    broadcast_result.store(broadcast_out);
    varying_result.store(varying_out);
    for (size_t i = 0; i < FLOAT_VEC_WIDTH; ++i) {
        EXPECT_FLOAT_EQ(broadcast_out[i], 4.0f);
        EXPECT_FLOAT_EQ(varying_out[i], 6.0f);
    }
}

// load_aligned/store_aligned only exist on the intrinsic specializations
// (SSE2/AVX2/AVX512); the scalar fallback exposes plain load/store instead.
#if defined(HPC_HAS_SSE2) || defined(HPC_HAS_AVX2) || defined(HPC_HAS_AVX512)
TEST(SimdVecTest, AlignedLoadStoreRoundTrip) {
    constexpr size_t kLanes = FLOAT_VEC_WIDTH * 4;
    auto src = make_aligned_vector<float>(kLanes);
    auto dst = make_aligned_vector<float>(kLanes, 0.0f);
    ASSERT_TRUE(is_aligned(src.data(), get_simd_alignment()));
    ASSERT_TRUE(is_aligned(dst.data(), get_simd_alignment()));

    for (size_t i = 0; i < kLanes; ++i) {
        src[i] = static_cast<float>(i) * 0.25f - 10.0f;
    }

    for (size_t i = 0; i < kLanes; i += FLOAT_VEC_WIDTH) {
        FloatVec::load_aligned(src.data() + i).store_aligned(dst.data() + i);
    }

    for (size_t i = 0; i < kLanes; ++i) {
        EXPECT_FLOAT_EQ(dst[i], src[i]);
    }
}
#endif

}  // namespace hpc::simd::test
