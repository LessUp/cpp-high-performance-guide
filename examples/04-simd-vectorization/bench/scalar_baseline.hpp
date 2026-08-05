/**
 * @file scalar_baseline.hpp
 * @brief Scalar reference implementations for SIMD benchmarks
 *
 * These live in a separate translation unit (scalar_baseline.cpp) that CMake
 * compiles with the auto-vectorizer disabled. If they shared simd_bench.cpp's
 * translation unit, `-O3 -march=native -mavx2` would silently auto-vectorize
 * them and the "scalar" baseline would actually be SIMD code, making the
 * speedup comparison meaningless.
 */

#pragma once

#include <cstddef>

namespace scalar_baseline {

void add_arrays(const float* a, const float* b, float* c, size_t n);
float dot_product(const float* a, const float* b, size_t n);
void scale_array(float* arr, float scalar, size_t n);
void clamp_array(float* arr, float lo, float hi, size_t n);

}  // namespace scalar_baseline
