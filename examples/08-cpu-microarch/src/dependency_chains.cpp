/**
 * @file dependency_chains.cpp
 * @brief Latency vs throughput: a dependent chain is limited by operation
 *        latency no matter how wide the CPU is, while independent chains let
 *        the out-of-order backend overlap work and approach throughput.
 *
 * Four kernels (microarch_kernels.cpp, scalar, auto-vectorization off):
 *   dep_add_chain / ilp4_add   — FP add latency vs 4-way ILP
 *   dep_mul_chain / ilp4_mul   — FP multiply latency vs 4-way ILP
 *
 * If PMU counters are available (hpc/pmu.hpp) cycles come from hardware;
 * otherwise wall time is reported and the cycle column is left out.
 */

#include <chrono>
#include <cstdio>
#include <hpc/pmu.hpp>

#include "microarch_kernels.hpp"

namespace {

constexpr std::size_t kIters = 100'000'000;

using Clock = std::chrono::steady_clock;

template <typename Fn>
double wall_seconds(Fn&& fn) {
    const auto start = Clock::now();
    fn();
    return std::chrono::duration<double>(Clock::now() - start).count();
}

// ops_per_iter: dependent chains do 1 FP op per loop iteration; the ilp4
// kernels do 4 independent ones, so cycles per FP op divide accordingly.
void run_case(const char* name, double (*kernel)(double, std::size_t), std::size_t ops_per_iter) {
    double result = 0.0;
    double wall = 0.0;
    const double total_ops = static_cast<double>(kIters) * static_cast<double>(ops_per_iter);

#if defined(__linux__)
    const hpc::pmu::Sample sample =
        hpc::pmu::measure([&] { wall = wall_seconds([&] { result = kernel(1.0, kIters); }); });
    const double ns_per_op = wall * 1e9 / total_ops;
    if (sample.available) {
        const double cycles_per_op = static_cast<double>(sample.cpu_cycles) / total_ops;
        std::printf("  %-18s %6.2f ns/op  %6.2f cycles/op  (IPC %.2f)\n", name, ns_per_op,
                    cycles_per_op, sample.ipc());
    } else {
        std::printf("  %-18s %6.2f ns/op  (PMU unavailable, no cycle data)\n", name, ns_per_op);
    }
#else
    wall = wall_seconds([&] { result = kernel(1.0, kIters); });
    std::printf("  %-18s %6.2f ns/op\n", name, wall * 1e9 / total_ops);
#endif

    if (result == 12345.678) {  // never true; keeps the kernel call alive
        std::printf("%f\n", result);
    }
}

}  // namespace

int main() {
    std::printf("latency vs ILP over %zu iterations per kernel\n", kIters);
    std::printf("dependent chain = latency bound; ilp4 = overlapped by the OoO backend\n");

    run_case("dep add chain", microarch::dep_add_chain, 1);
    run_case("ilp4 add", microarch::ilp4_add, 4);
    run_case("dep mul chain", microarch::dep_mul_chain, 1);
    run_case("ilp4 mul", microarch::ilp4_mul, 4);

#if defined(__linux__)
    if (!hpc::pmu::available()) {
        hpc::pmu::print_unavailable_notice("dependency_chains");
    }
#endif
    std::printf("dependency_chains: OK\n");
    return 0;
}
