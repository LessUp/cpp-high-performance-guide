/**
 * @file scalar_baseline.cpp
 * @brief Scalar reference implementation (compiled WITHOUT auto-vectorization)
 *
 * CMake disables the vectorizer for this translation unit only
 * (-fno-tree-vectorize on GCC, -fno-vectorize -fno-slp-vectorize on Clang).
 * See scalar_baseline.hpp for rationale.
 */

#include "scalar_baseline.hpp"

namespace scalar_baseline {

void add_arrays(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

}  // namespace scalar_baseline
