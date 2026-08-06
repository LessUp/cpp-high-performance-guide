/**
 * @file numa_binding.cpp
 * @brief NUMA awareness with libnuma: topology discovery and node-local
 *        allocation.
 *
 * On multi-socket machines each CPU socket owns "local" memory; touching
 * memory attached to another node goes over the interconnect and costs
 * roughly 1.5-2x the local latency. HPC runtimes therefore bind both the
 * thread (affinity) and its allocations (numa_alloc_onnode / numactl
 * --membind) to the same node.
 *
 * This example prints the NUMA topology and measures a streaming write on
 * node-local vs interleaved allocations. Single-node machines still run it:
 * the two allocations then behave identically, which is the correct result.
 *
 * Requires libnuma (configure-time gate; skipped when absent).
 */

#include <numa.h>
#include <numaif.h>
#include <sched.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>

namespace {

constexpr std::size_t kBytes = 256u << 20;  // 256 MiB per buffer

using Clock = std::chrono::steady_clock;

double stream_write_mib_per_s(volatile unsigned char* buf, std::size_t bytes) {
    const auto start = Clock::now();
    for (std::size_t i = 0; i < bytes; i += 64) {
        buf[i] = static_cast<unsigned char>(i);
    }
    const double seconds = std::chrono::duration<double>(Clock::now() - start).count();
    return static_cast<double>(bytes >> 20) / seconds;
}

void print_topology() {
    const int nodes = numa_num_configured_nodes();
    std::printf("NUMA topology: %d configured node(s)\n", nodes);
    for (int node = 0; node < nodes; ++node) {
        struct bitmask* cpus = numa_allocate_cpumask();
        if (numa_node_to_cpus(node, cpus) == 0) {
            int count = 0;
            for (int cpu = 0; cpu < numa_num_possible_cpus(); ++cpu) {
                if (numa_bitmask_isbitset(cpus, static_cast<unsigned int>(cpu))) {
                    ++count;
                }
            }
            std::printf("  node %d: %d CPU(s)\n", node, count);
        }
        numa_free_cpumask(cpus);
    }
}

}  // namespace

int main() {
    if (numa_available() < 0) {
        std::printf("numa_binding: NUMA not available on this kernel, skipping.\n");
        return 0;
    }

    print_topology();

    // numa_preferred() returns -1 without an active policy; fall back to the
    // node the current CPU belongs to, then to node 0.
    int node = numa_node_of_cpu(sched_getcpu());
    if (node < 0) {
        node = numa_preferred();
    }
    if (node < 0) {
        node = 0;
    }
    std::printf("allocating %zu MiB node-local on node %d and interleaved...\n", kBytes >> 20,
                node);

    // Node-local: pages come from one node's memory.
    void* local_mem = numa_alloc_onnode(kBytes, node);
    // Interleaved: pages round-robin across all nodes.
    void* inter_mem = numa_alloc_interleaved(kBytes);
    if (local_mem == nullptr || inter_mem == nullptr) {
        std::fprintf(stderr, "numa allocation failed\n");
        return 1;
    }

    const double local_bw = stream_write_mib_per_s(static_cast<unsigned char*>(local_mem), kBytes);
    const double inter_bw = stream_write_mib_per_s(static_cast<unsigned char*>(inter_mem), kBytes);

    std::printf("  node-local write bandwidth:  %8.1f MiB/s\n", local_bw);
    std::printf("  interleaved write bandwidth: %8.1f MiB/s\n", inter_bw);

    const int nodes = numa_num_configured_nodes();
    if (nodes <= 1) {
        std::printf("single NUMA node: both allocations are equivalent by design\n");
    } else if (inter_bw > local_bw * 1.05) {
        // Interleaving should not beat local memory; warn but do not fail —
        // scheduling noise on shared machines can produce odd numbers.
        std::printf("note: interleaved beat node-local; re-run on an idle machine\n");
    }

    numa_free(local_mem, kBytes);
    numa_free(inter_mem, kBytes);

    std::printf("numa_binding: OK\n");
    return 0;
}
