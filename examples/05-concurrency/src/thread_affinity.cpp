/**
 * @file thread_affinity.cpp
 * @brief Pinning threads to CPUs with pthread_setaffinity_np.
 *
 * Without pinning, the OS migrates threads between cores. Migration
 * invalidates local caches and (on NUMA machines) can move a thread away
 * from the memory it touches, so latency-sensitive workers (network loops,
 * spin-wait queues, per-core shards) are usually pinned explicitly.
 *
 * This example pins N worker threads one-per-core, verifies the mask with
 * pthread_getaffinity_np, and shows each thread observing its own CPU.
 * Linux-only; other platforms print a notice.
 */

#if !defined(__linux__)
#include <cstdio>
int main() {
    std::printf("thread_affinity requires Linux (pthread_setaffinity_np).\n");
    return 0;
}
#else

#include <pthread.h>
#include <sched.h>

#include <atomic>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

namespace {

std::string mask_to_string(const cpu_set_t& mask) {
    std::string out;
    char buf[8];
    for (int cpu = 0; cpu < CPU_SETSIZE; ++cpu) {
        if (CPU_ISSET(cpu, &mask)) {
            if (!out.empty()) {
                out += ",";
            }
            std::snprintf(buf, sizeof(buf), "%d", cpu);
            out += buf;
        }
    }
    return out.empty() ? "(none)" : out;
}

}  // namespace

int main() {
    const int hardware = std::thread::hardware_concurrency();
    const int workers = hardware > 0 ? hardware : 4;

    std::printf("thread_affinity: %d hardware threads, pinning %d workers\n", hardware, workers);

    std::atomic<int> failures{0};
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(workers));

    for (int i = 0; i < workers; ++i) {
        threads.emplace_back([i, &failures] {
            // Pin this thread to exactly one CPU.
            cpu_set_t mask;
            CPU_ZERO(&mask);
            CPU_SET(i % CPU_SETSIZE, &mask);
            if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) != 0) {
                std::fprintf(stderr, "worker %d: setaffinity failed\n", i);
                failures.fetch_add(1, std::memory_order_relaxed);
                return;
            }

            // Read the mask back and confirm it is what we asked for.
            cpu_set_t actual;
            CPU_ZERO(&actual);
            if (pthread_getaffinity_np(pthread_self(), sizeof(actual), &actual) != 0 ||
                !CPU_ISSET(i % CPU_SETSIZE, &actual) || CPU_COUNT(&actual) != 1) {
                std::fprintf(stderr, "worker %d: affinity mask not applied\n", i);
                failures.fetch_add(1, std::memory_order_relaxed);
                return;
            }

            // sched_getcpu() should agree with the pin (a migrated thread
            // would report a different CPU).
            const int cpu = sched_getcpu();
            if (cpu != i % CPU_SETSIZE) {
                std::fprintf(stderr, "worker %d: running on cpu %d\n", i, cpu);
                failures.fetch_add(1, std::memory_order_relaxed);
                return;
            }

            if (i < 8) {  // keep output readable on big machines
                std::printf("  worker %d pinned to CPU %d (mask %s)\n", i, cpu,
                            mask_to_string(actual).c_str());
            }
        });
    }

    for (std::thread& t : threads) {
        t.join();
    }

    if (workers > 8) {
        std::printf("  ... (%d more workers pinned successfully)\n", workers - 8);
    }
    std::printf("thread_affinity: %s\n", failures.load() == 0 ? "OK" : "FAILED");
    return failures.load() == 0 ? 0 : 1;
}

#endif  // __linux__
