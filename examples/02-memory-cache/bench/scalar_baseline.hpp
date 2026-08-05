/**
 * @file scalar_baseline.hpp
 * @brief Scalar reference implementation for the alignment benchmark
 *
 * Lives in a separate translation unit (scalar_baseline.cpp) compiled with
 * the auto-vectorizer disabled, so the "scalar" baseline in the aligned vs
 * unaligned comparison is not secretly auto-vectorized by -O3 -march=native.
 */

#pragma once

#include <cstddef>

namespace scalar_baseline {

void add_arrays(const float* a, const float* b, float* c, size_t n);

}  // namespace scalar_baseline
