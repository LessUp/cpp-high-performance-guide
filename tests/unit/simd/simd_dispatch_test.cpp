#include <gtest/gtest.h>

#include <hpc/simd.hpp>

namespace hpc::simd {

namespace {

TEST(SimdDispatchTest, MatchesScalarReferenceForTailLength) {
    constexpr size_t kSize = 21;
    aligned_vector<float> a(kSize), b(kSize), actual(kSize), expected(kSize);

    for (size_t i = 0; i < kSize; ++i) {
        a[i] = static_cast<float>(i) * 1.5f;
        b[i] = static_cast<float>(kSize - i) * 0.25f;
        expected[i] = a[i] + b[i];
    }

    dispatch_add_arrays(a.data(), b.data(), actual.data(), kSize);

    for (size_t i = 0; i < kSize; ++i) {
        EXPECT_FLOAT_EQ(actual[i], expected[i]);
    }
}

}  // namespace

}  // namespace hpc::simd
