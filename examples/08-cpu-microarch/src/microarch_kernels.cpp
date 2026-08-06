#include "microarch_kernels.hpp"

namespace microarch {

long long sum_branched(const int* data, std::size_t n, int threshold) {
    long long sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        if (data[i] >= threshold) {  // data-dependent branch: predictable only
            sum += data[i];          // when the input order is predictable
        }
    }
    return sum;
}

// This translation unit is compiled with -fno-if-conversion (see the module
// CMakeLists) so that sum_branched keeps a real branch. That flag would also
// demote this ternary to a branch, so re-enable if-conversion for this one
// function: the point of the demo is exactly "branch vs cmov".
#if defined(__GNUC__) && !defined(__clang__)
__attribute__((optimize("if-conversion")))
#endif
long long sum_ternary(const int* data, std::size_t n, int threshold) {
    long long sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // Lowered to a conditional move (cmov): no control-flow dependency,
        // nothing to mispredict.
        sum += data[i] >= threshold ? data[i] : 0;
    }
    return sum;
}

long long sum_masked(const int* data, std::size_t n, int threshold) {
    long long sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // Pure arithmetic mask: guaranteed branchless even at -O0-ish
        // settings. -(cond) is all-ones when true, zero when false.
        const int v = data[i];
        sum += v & -(v >= threshold);
    }
    return sum;
}

// Dependent chain: each add waits for the previous result. Throughput is
// limited by add latency, no matter how wide the OoO backend is.
double dep_add_chain(double seed, std::size_t iters) {
    double x = seed;
    for (std::size_t i = 0; i < iters; ++i) {
        x = x + 1.0000001;
    }
    return x;
}

// Four independent chains: the OoO backend overlaps them, so total time is
// ~latency + iters/4 * latency instead of iters * latency (until width runs
// out). The final combine keeps the accumulators live.
double ilp4_add(double seed, std::size_t iters) {
    double a = seed;
    double b = seed;
    double c = seed;
    double d = seed;
    for (std::size_t i = 0; i < iters; ++i) {
        a = a + 1.0000001;
        b = b + 1.0000002;
        c = c + 1.0000003;
        d = d + 1.0000004;
    }
    return a + b + c + d;
}

double dep_mul_chain(double seed, std::size_t iters) {
    double x = seed;
    for (std::size_t i = 0; i < iters; ++i) {
        x = x * 1.0000001;  // multiply latency > add latency on most cores
    }
    return x;
}

double ilp4_mul(double seed, std::size_t iters) {
    double a = seed;
    double b = seed;
    double c = seed;
    double d = seed;
    for (std::size_t i = 0; i < iters; ++i) {
        a = a * 1.0000001;
        b = b * 1.0000002;
        c = c * 1.0000003;
        d = d * 1.0000004;
    }
    return a + b + c + d;
}

}  // namespace microarch
