/**
 * @file scalar_baseline.cpp
 * @brief Scalar reference implementations (compiled WITHOUT auto-vectorization)
 *
 * CMake disables the vectorizer for this translation unit only
 * (-fno-tree-vectorize on GCC, -fno-vectorize -fno-slp-vectorize on Clang),
 * so these stay genuinely scalar even though the benchmark target is built
 * with -O3 -march=native -mavx2. See scalar_baseline.hpp for rationale.
 */

#include "scalar_baseline.hpp"

#include <algorithm>

namespace scalar_baseline {

void add_arrays(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

float dot_product(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

void scale_array(float* arr, float scalar, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] *= scalar;
    }
}

void clamp_array(float* arr, float lo, float hi, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] = std::max(lo, std::min(hi, arr[i]));
    }
}

}  // namespace scalar_baseline
