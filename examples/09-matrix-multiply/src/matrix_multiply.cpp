/**
 * @file matrix_multiply.cpp
 * @brief The GEMM case study: naive -> cache tiling -> SIMD(FMA) -> OpenMP,
 *        measured in GFLOPS and verified against the naive result.
 *
 * Matrix multiply is the canonical optimization walkthrough because every
 * layer of the machine shows up: memory hierarchy (tiling), SIMD width
 * (FMA micro-kernel), and cores (OpenMP). See the deep-dive doc
 * docs/zh/deep-dives/gemm-case-study.md.
 */

#include <chrono>
#include <cmath>
#include <cstdio>
#include <hpc/gemm.hpp>
#include <random>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

template <typename Fn>
double best_seconds(Fn&& fn, int repeats) {
    double best = 1e18;
    for (int r = 0; r < repeats; ++r) {
        const auto start = Clock::now();
        fn();
        best = std::min(best, std::chrono::duration<double>(Clock::now() - start).count());
    }
    return best;
}

bool matches_reference(const std::vector<float>& actual, const std::vector<float>& expected) {
    for (std::size_t i = 0; i < actual.size(); ++i) {
        const float diff = std::fabs(actual[i] - expected[i]);
        const float scale = std::max(1.0f, std::fabs(expected[i]));
        if (diff > 1e-3f * scale) {
            return false;
        }
    }
    return true;
}

}  // namespace

int main() {
    constexpr int kSizes[] = {256, 512};
    constexpr int kRepeats = 3;

    for (const int n : kSizes) {
        const std::size_t elements = static_cast<std::size_t>(n) * n;
        const double flops = 2.0 * n * n * n;

        std::vector<float> a(elements), b(elements);
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (std::size_t i = 0; i < elements; ++i) {
            a[i] = dist(rng);
            b[i] = dist(rng);
        }

        std::vector<float> reference(elements), c(elements);
        hpc::gemm::zero(reference.data(), n);
        hpc::gemm::naive(a.data(), b.data(), reference.data(), n);

        std::printf("GEMM %dx%d (%.0f MFLOP per run)\n", n, n, flops / 1e6);

        struct Stage {
            const char* name;
            void (*fn)(const float*, const float*, float*, int);
        };
        // Lambdas adapt the tiled/simd/parallel overloads (they carry a
        // default block_size parameter, which does not convert to a plain
        // function pointer).
        const Stage stages[] = {
            {"naive", hpc::gemm::naive},
            {"tiled", +[](const float* ma, const float* mb, float* mc,
                          int m) { hpc::gemm::tiled(ma, mb, mc, m); }},
            {"simd (FMA)", +[](const float* ma, const float* mb, float* mc,
                               int m) { hpc::gemm::simd(ma, mb, mc, m); }},
            {"parallel (OpenMP)", +[](const float* ma, const float* mb, float* mc,
                                      int m) { hpc::gemm::parallel(ma, mb, mc, m); }},
        };

        double naive_gflops = 0.0;
        for (const Stage& stage : stages) {
            hpc::gemm::zero(c.data(), n);
            const double seconds = best_seconds(
                [&] {
                    hpc::gemm::zero(c.data(), n);
                    stage.fn(a.data(), b.data(), c.data(), n);
                },
                kRepeats);
            if (!matches_reference(c, reference)) {
                std::fprintf(stderr, "%s produced wrong result\n", stage.name);
                return 1;
            }
            const double gflops = flops / seconds / 1e9;
            if (naive_gflops == 0.0) {
                naive_gflops = gflops;
            }
            std::printf("  %-22s %8.2f ms  %8.2f GFLOPS  (%.2fx vs naive)\n", stage.name,
                        seconds * 1e3, gflops, gflops / naive_gflops);
        }

        const int threads =
#ifdef _OPENMP
            omp_get_max_threads();
#else
            1;
#endif
        std::printf("  (verified against naive; OpenMP threads: %d)\n\n", threads);
    }

    std::printf("matrix_multiply: OK\n");
    return 0;
}
