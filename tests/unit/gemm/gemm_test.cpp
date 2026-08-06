/**
 * @file gemm_test.cpp
 * @brief Cross-stage correctness tests for hpc::gemm. Accumulation order
 *        differs between stages, so comparisons use a relative tolerance.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <hpc/gemm.hpp>
#include <random>
#include <vector>

namespace hpc::gemm::test {

namespace {

std::vector<float> random_matrix(int n, unsigned seed) {
    std::vector<float> v(static_cast<std::size_t>(n) * n);
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (float& x : v) {
        x = dist(rng);
    }
    return v;
}

void expect_close(const std::vector<float>& actual, const std::vector<float>& expected) {
    ASSERT_EQ(actual.size(), expected.size());
    for (std::size_t i = 0; i < actual.size(); ++i) {
        const float diff = std::fabs(actual[i] - expected[i]);
        const float scale = std::max(1.0f, std::fabs(expected[i]));
        EXPECT_LE(diff, 1e-3f * scale) << "element " << i;
    }
}

template <typename Stage>
void check_stage_against_naive(int n, Stage stage) {
    const std::vector<float> a = random_matrix(n, 1);
    const std::vector<float> b = random_matrix(n, 2);
    std::vector<float> reference(static_cast<std::size_t>(n) * n, 0.0f);
    std::vector<float> actual(static_cast<std::size_t>(n) * n, 0.0f);

    naive(a.data(), b.data(), reference.data(), n);
    stage(a.data(), b.data(), actual.data(), n);
    expect_close(actual, reference);
}

}  // namespace

TEST(GemmTest, SmallSizes) {
    // Sizes that are not multiples of the tile (64) or vector width (8/4)
    // exercise every tail path.
    for (const int n : {1, 2, 7, 8, 9, 33, 64, 65, 100}) {
        check_stage_against_naive(
            n, [](const float* a, const float* b, float* c, int m) { tiled(a, b, c, m); });
        check_stage_against_naive(
            n, [](const float* a, const float* b, float* c, int m) { simd(a, b, c, m); });
        check_stage_against_naive(
            n, [](const float* a, const float* b, float* c, int m) { parallel(a, b, c, m); });
    }
}

TEST(GemmTest, KnownValues) {
    // [1 2; 3 4] * [5 6; 7 8] = [19 22; 43 50]
    const float a[4] = {1, 2, 3, 4};
    const float b[4] = {5, 6, 7, 8};
    float c[4] = {};

    simd(a, b, c, 2);
    EXPECT_FLOAT_EQ(c[0], 19.0f);
    EXPECT_FLOAT_EQ(c[1], 22.0f);
    EXPECT_FLOAT_EQ(c[2], 43.0f);
    EXPECT_FLOAT_EQ(c[3], 50.0f);

    float c2[4] = {};
    parallel(a, b, c2, 2);
    EXPECT_FLOAT_EQ(c2[0], 19.0f);
    EXPECT_FLOAT_EQ(c2[3], 50.0f);
}

TEST(GemmTest, TiledCustomBlockSize) {
    const std::vector<float> a = random_matrix(70, 3);
    const std::vector<float> b = random_matrix(70, 4);
    std::vector<float> reference(70 * 70, 0.0f);
    std::vector<float> actual(70 * 70, 0.0f);

    naive(a.data(), b.data(), reference.data(), 70);
    tiled(a.data(), b.data(), actual.data(), 70, /*block_size=*/16);
    expect_close(actual, reference);
}

}  // namespace hpc::gemm::test
