#pragma once

#include <cstddef>

/**
 * Scalar kernels for the microarchitecture examples. These live in their own
 * translation unit (microarch_kernels.cpp) which CMake compiles with the
 * auto-vectorizer disabled: the whole point is to measure *scalar*
 * branch-prediction and ILP effects, and -O3 -march=native would otherwise
 * vectorize the conditional-sum and accumulator loops, erasing the effect.
 */

namespace microarch {

// Conditional sums used by the branch-prediction demo. All three compute the
// same result; only the control-flow shape differs.
long long sum_branched(const int* data, std::size_t n, int threshold);
long long sum_ternary(const int* data, std::size_t n, int threshold);
long long sum_masked(const int* data, std::size_t n, int threshold);

// Latency/ILP chains used by the dependency demo. `iters` loop iterations,
// all starting from the same seed so results stay comparable.
double dep_add_chain(double seed, std::size_t iters);
double ilp4_add(double seed, std::size_t iters);
double dep_mul_chain(double seed, std::size_t iters);
double ilp4_mul(double seed, std::size_t iters);

}  // namespace microarch
