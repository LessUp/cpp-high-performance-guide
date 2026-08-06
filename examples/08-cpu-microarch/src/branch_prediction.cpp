/**
 * @file branch_prediction.cpp
 * @brief The classic sorted-vs-unsorted experiment: data-dependent branches
 *        cost ~15-20 cycles per misprediction, unless the pattern is
 *        predictable. Branchless rewrites remove the cost entirely.
 *
 * Three identical conditional sums are compared:
 *   branched  — if (v >= t) sum += v;      unpredictable on random order
 *   ternary   — sum += v >= t ? v : 0;     compiles to cmov, no branch
 *   masked    — sum += v & -(v >= t);      pure arithmetic, always branchless
 *
 * The kernels live in microarch_kernels.cpp, compiled without
 * auto-vectorization (see that file's header for why).
 */

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>

#include "microarch_kernels.hpp"

namespace {

using Clock = std::chrono::steady_clock;

constexpr std::size_t kElements = 1u << 20;
constexpr int kRepeats = 10;
constexpr int kThreshold = 128;

template <typename Fn>
double best_of(Fn&& fn) {
    double best = 1e18;
    for (int r = 0; r < kRepeats; ++r) {
        const auto start = Clock::now();
        fn();
        best = std::min(best, std::chrono::duration<double>(Clock::now() - start).count());
    }
    return best;
}

void report(const char* name, double seconds, double baseline, long long value) {
    std::printf("  %-34s %8.2f ms  (%.2fx vs branched-unsorted)  sum=%lld\n", name, seconds * 1e3,
                baseline / seconds, value);
}

}  // namespace

int main() {
    std::vector<int> data(kElements);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 255);
    for (int& v : data) {
        v = dist(rng);
    }

    std::vector<int> sorted = data;
    std::sort(sorted.begin(), sorted.end());

    std::printf("branch prediction: conditional sum of %zu bytes (threshold %d, best of %d)\n",
                kElements, kThreshold, kRepeats);

    // Reference numbers on the random (unpredictable) order.
    const long long expect = microarch::sum_branched(data.data(), data.size(), kThreshold);

    double t = 0.0;
    const double branched_unsorted =
        best_of([&] { t = microarch::sum_branched(data.data(), data.size(), kThreshold); });
    if (t != expect) {
        std::fprintf(stderr, "branched(unsorted) wrong sum\n");
        return 1;
    }
    report("branched, unsorted (random)", branched_unsorted, branched_unsorted, t);

    const double branched_sorted =
        best_of([&] { t = microarch::sum_branched(sorted.data(), sorted.size(), kThreshold); });
    if (t != expect) {
        std::fprintf(stderr, "branched(sorted) wrong sum\n");
        return 1;
    }
    report("branched, sorted (predictable)", branched_sorted, branched_unsorted, t);

    const double ternary_unsorted =
        best_of([&] { t = microarch::sum_ternary(data.data(), data.size(), kThreshold); });
    if (t != expect) {
        std::fprintf(stderr, "ternary wrong sum\n");
        return 1;
    }
    report("ternary (cmov), unsorted", ternary_unsorted, branched_unsorted, t);

    const double masked_unsorted =
        best_of([&] { t = microarch::sum_masked(data.data(), data.size(), kThreshold); });
    if (t != expect) {
        std::fprintf(stderr, "masked wrong sum\n");
        return 1;
    }
    report("masked arithmetic, unsorted", masked_unsorted, branched_unsorted, t);

    std::printf("branch_prediction: OK (all variants agree, sum=%lld)\n", expect);
    return 0;
}
