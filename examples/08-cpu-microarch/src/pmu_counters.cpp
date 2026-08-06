/**
 * @file pmu_counters.cpp
 * @brief Hardware performance counters from inside the process via
 *        perf_event_open — the same PMU that `perf stat` uses.
 *
 * Measures cycles, instructions (→ IPC), cache misses and branch misses for
 * a scalar add loop vs the runtime-dispatched SIMD kernel from hpc/simd.hpp,
 * over a working set that deliberately exceeds L2 so cache misses show up.
 *
 * On machines without PMU access (WSL2/VM/perf_event_paranoid) the demo
 * prints a notice and exits cleanly — the wrapper is designed to degrade.
 */

#include <cstdio>
#include <hpc/pmu.hpp>
#include <hpc/simd.hpp>
#include <random>
#include <vector>

namespace {

constexpr std::size_t kElements = 8u << 20;  // 3 x 32 MiB arrays >> L2

void scalar_add(const float* a, const float* b, float* c, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void print_sample(const char* name, const hpc::pmu::Sample& s) {
    std::printf(
        "  %-22s cycles %12llu  instr %12llu  IPC %5.2f  cache-miss %10llu  "
        "branch-miss %8llu\n",
        name, static_cast<unsigned long long>(s.cpu_cycles),
        static_cast<unsigned long long>(s.instructions), s.ipc(),
        static_cast<unsigned long long>(s.cache_misses),
        static_cast<unsigned long long>(s.branch_misses));
}

}  // namespace

int main() {
#if !defined(__linux__)
    std::printf("pmu_counters requires Linux (perf_event_open).\n");
    return 0;
#else
    if (!hpc::pmu::available()) {
        hpc::pmu::print_unavailable_notice("pmu_counters");
        return 0;
    }

    std::vector<float> a(kElements), b(kElements), c(kElements);
    std::mt19937 rng(7);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (std::size_t i = 0; i < kElements; ++i) {
        a[i] = dist(rng);
        b[i] = dist(rng);
    }

    std::printf("PMU counters over %zu-element add (working set 96 MiB, beyond L2)\n", kElements);

    const hpc::pmu::Sample scalar_sample =
        hpc::pmu::measure([&] { scalar_add(a.data(), b.data(), c.data(), kElements); });
    print_sample("scalar loop", scalar_sample);

    const hpc::pmu::Sample simd_sample =
        hpc::pmu::measure([&] { hpc::simd::add_arrays(a.data(), b.data(), c.data(), kElements); });
    print_sample("simd dispatched", simd_sample);

    // Sanity: SIMD processes ~4x the work per instruction, so its IPC-based
    // instruction count should be clearly lower for the same bytes moved.
    if (simd_sample.instructions >= scalar_sample.instructions) {
        std::printf(
            "note: SIMD did not reduce instruction count on this run "
            "(unexpected but not fatal)\n");
    }
    std::printf("pmu_counters: OK\n");
    return 0;
#endif
}
