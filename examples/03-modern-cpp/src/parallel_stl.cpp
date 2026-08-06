/**
 * @file parallel_stl.cpp
 * @brief C++17 parallel STL execution policies: seq, unseq, par, par_unseq.
 *
 * The execution policies passed to algorithms like std::transform/std::reduce
 * tell the implementation what it may assume:
 *
 *   seq        — in-order, single-threaded. Mostly a portability baseline.
 *   unseq      — single-threaded, but iterations may be vectorized and
 *                interleaved. GCC/Clang turn this into SIMD code.
 *   par        — multi-threaded, iterations may run in any order on any
 *                thread.
 *   par_unseq  — multi-threaded AND vectorized inside each thread.
 *
 * On GCC's libstdc++, par/par_unseq are implemented on top of Intel TBB.
 * When CMake does not find TBB this example still builds and demonstrates
 * seq/unseq; the par policies are compiled in only with -DHPC_HAS_TBB.
 *
 * Timing here is educational (steady_clock over a few repeats). For
 * controlled measurements see bench/parallel_stl_bench.cpp.
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <execution>
#include <numeric>
#include <vector>

namespace {

constexpr std::size_t kElements = 8u << 20;  // 8 M floats = 32 MiB
constexpr int kRepeats = 5;

// Elementwise work heavy enough that the loop is not purely memory-bound:
// sqrt prevents trivial strength reduction while staying deterministic.
float workload(float x) {
    return std::sqrt(x) * 0.5f + 1.0f;
}

template <typename Policy>
double measure(Policy policy, const std::vector<float>& in, std::vector<float>& out) {
    double best_ms = 1e18;
    for (int i = 0; i < kRepeats; ++i) {
        const auto start = std::chrono::steady_clock::now();
        std::transform(policy, in.begin(), in.end(), out.begin(),
                       [](float x) { return workload(x); });
        const auto stop = std::chrono::steady_clock::now();
        best_ms =
            std::min(best_ms, std::chrono::duration<double, std::milli>(stop - start).count());
    }
    return best_ms;
}

double measure_loop(const std::vector<float>& in, std::vector<float>& out) {
    double best_ms = 1e18;
    for (int i = 0; i < kRepeats; ++i) {
        const auto start = std::chrono::steady_clock::now();
        for (std::size_t j = 0; j < in.size(); ++j) {
            out[j] = workload(in[j]);
        }
        const auto stop = std::chrono::steady_clock::now();
        best_ms =
            std::min(best_ms, std::chrono::duration<double, std::milli>(stop - start).count());
    }
    return best_ms;
}

bool verify(const std::vector<float>& in, const std::vector<float>& out) {
    for (std::size_t i = 0; i < out.size(); ++i) {
        const float expected = workload(in[i]);
        if (std::fabs(out[i] - expected) > 1e-4f * std::fabs(expected)) {
            return false;
        }
    }
    return true;
}

void report(const char* name, double ms, double baseline_ms) {
    std::printf("  %-28s %8.2f ms  (%.2fx vs plain loop)\n", name, ms, baseline_ms / ms);
}

}  // namespace

int main() {
    std::vector<float> in(kElements), out(kElements, 0.0f);
    std::iota(in.begin(), in.end(), 0.0f);

    std::printf("parallel STL std::transform over %zu floats (%d repeats, best kept)\n", kElements,
                kRepeats);

    const double loop_ms = measure_loop(in, out);
    report("plain loop", loop_ms, loop_ms);
    if (!verify(in, out)) {
        std::fprintf(stderr, "plain loop produced wrong results\n");
        return 1;
    }

    const double seq_ms = measure(std::execution::seq, in, out);
    report("std::execution::seq", seq_ms, loop_ms);
    if (!verify(in, out)) {
        std::fprintf(stderr, "seq produced wrong results\n");
        return 1;
    }

    const double unseq_ms = measure(std::execution::unseq, in, out);
    report("std::execution::unseq", unseq_ms, loop_ms);
    if (!verify(in, out)) {
        std::fprintf(stderr, "unseq produced wrong results\n");
        return 1;
    }

#ifdef HPC_HAS_TBB
    const double par_ms = measure(std::execution::par, in, out);
    report("std::execution::par (TBB)", par_ms, loop_ms);
    if (!verify(in, out)) {
        std::fprintf(stderr, "par produced wrong results\n");
        return 1;
    }

    const double par_unseq_ms = measure(std::execution::par_unseq, in, out);
    report("std::execution::par_unseq (TBB)", par_unseq_ms, loop_ms);
    if (!verify(in, out)) {
        std::fprintf(stderr, "par_unseq produced wrong results\n");
        return 1;
    }
#else
    std::printf("  %-28s %8s\n", "std::execution::par", "n/a (build with TBB)");
    std::printf("  %-28s %8s\n", "std::execution::par_unseq", "n/a (build with TBB)");
    // Honest note: with -O3 -march=native the plain loop above is already
    // auto-vectorized, so unseq has no headroom left. The multi-threaded
    // speedup of this workload comes from par/par_unseq, which need TBB.
#endif

    std::printf("parallel_stl: OK\n");
    return 0;
}
