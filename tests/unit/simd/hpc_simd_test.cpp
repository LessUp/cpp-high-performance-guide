#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <hpc/simd.hpp>

namespace hpc::simd::test {

namespace {

SIMDLevel expected_runtime_level() {
#if (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    return SIMDLevel::NEON;
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __builtin_cpu_init();
    if (__builtin_cpu_supports("avx512f")) {
        return SIMDLevel::AVX512;
    }
    if (__builtin_cpu_supports("avx2")) {
        return SIMDLevel::AVX2;
    }
    if (__builtin_cpu_supports("avx")) {
        return SIMDLevel::AVX;
    }
    if (__builtin_cpu_supports("sse2")) {
        return SIMDLevel::SSE2;
    }
#endif
    return SIMDLevel::Scalar;
}

size_t expected_alignment(SIMDLevel level) {
    switch (level) {
        case SIMDLevel::AVX512:
            return 64;
        case SIMDLevel::AVX2:
        case SIMDLevel::AVX:
            return 32;
        case SIMDLevel::SSE2:
        case SIMDLevel::NEON:
            return 16;
        case SIMDLevel::Scalar:
        default:
            return sizeof(void*);
    }
}

}  // namespace

TEST(HpcSimdTest, RuntimeLevelMatchesSupportedCapability) {
    EXPECT_EQ(detect_simd_level(), expected_runtime_level());
}

TEST(HpcSimdTest, RuntimeAlignmentMatchesReportedLevel) {
    const SIMDLevel level = detect_simd_level();
    EXPECT_EQ(get_simd_alignment(), expected_alignment(level));
}

TEST(HpcSimdTest, AddArraysHandlesTailLength) {
    constexpr size_t kSize = 21;
    aligned_vector<float> a(kSize), b(kSize), actual(kSize), expected(kSize);

    for (size_t i = 0; i < kSize; ++i) {
        a[i] = static_cast<float>(i) * 1.25f;
        b[i] = static_cast<float>(kSize - i) * 0.75f;
        expected[i] = a[i] + b[i];
    }

    add_arrays(a.data(), b.data(), actual.data(), kSize);

    for (size_t i = 0; i < kSize; ++i) {
        EXPECT_FLOAT_EQ(actual[i], expected[i]);
    }
}

TEST(HpcSimdTest, MultiplyDotScaleAndClampShareTheCanonicalModule) {
    constexpr size_t kSize = 37;
    aligned_vector<float> a(kSize), b(kSize), multiplied(kSize);

    for (size_t i = 0; i < kSize; ++i) {
        a[i] = static_cast<float>(i) - 10.0f;
        b[i] = static_cast<float>((i % 5) + 1);
    }

    multiply_arrays(a.data(), b.data(), multiplied.data(), kSize);
    for (size_t i = 0; i < kSize; ++i) {
        EXPECT_FLOAT_EQ(multiplied[i], a[i] * b[i]);
    }

    const float dot = dot_product(a.data(), b.data(), kSize);
    float expected_dot = 0.0f;
    for (size_t i = 0; i < kSize; ++i) {
        expected_dot += a[i] * b[i];
    }
    EXPECT_NEAR(dot, expected_dot, 1e-4f);

    scale_array(multiplied.data(), 0.5f, kSize);
    for (size_t i = 0; i < kSize; ++i) {
        EXPECT_FLOAT_EQ(multiplied[i], a[i] * b[i] * 0.5f);
    }

    clamp_array(multiplied.data(), -6.0f, 6.0f, kSize);
    for (size_t i = 0; i < kSize; ++i) {
        EXPECT_GE(multiplied[i], -6.0f);
        EXPECT_LE(multiplied[i], 6.0f);
    }
}

TEST(HpcSimdTest, DispatchAliasMatchesCanonicalAddKernel) {
    constexpr size_t kSize = 13;
    aligned_vector<float> a(kSize, 2.0f);
    aligned_vector<float> b(kSize, -0.5f);
    aligned_vector<float> actual(kSize, 0.0f);

    dispatch_add_arrays(a.data(), b.data(), actual.data(), kSize);

    for (float value : actual) {
        EXPECT_FLOAT_EQ(value, 1.5f);
    }
}

}  // namespace hpc::simd::test
