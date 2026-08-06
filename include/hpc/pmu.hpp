#pragma once

/**
 * @file pmu.hpp
 * @brief RAII wrappers over Linux PMU hardware counters (perf_event_open).
 *
 * Lets examples measure cycles/instructions/cache-misses/branch-misses in
 * process, which is what turns "it feels faster" into IPC and miss rates.
 *
 * Availability is never guaranteed: VMs, WSL2, containers and
 * perf_event_paranoid >= 3 can all refuse counters. Every entry point
 * therefore degrades gracefully — try_open() returns an invalid counter and
 * available() reports false instead of throwing.
 *
 * Linux-only. The header is empty on other platforms.
 */

#if defined(__linux__)

#include <cstdint>
#include <cstdio>
#include <initializer_list>

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace hpc::pmu {

enum class Event {
    CpuCycles,
    Instructions,
    CacheMisses,
    BranchMisses,
};

inline const char* event_name(Event event) {
    switch (event) {
        case Event::CpuCycles:
            return "cpu-cycles";
        case Event::Instructions:
            return "instructions";
        case Event::CacheMisses:
            return "cache-misses";
        case Event::BranchMisses:
            return "branch-misses";
    }
    return "unknown";
}

// One hardware counter. Move-only; invalid when the kernel refused the
// event (check valid() before trusting any value).
class Counter {
public:
    Counter() = default;

    Counter(const Counter&) = delete;
    Counter& operator=(const Counter&) = delete;

    Counter(Counter&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

    Counter& operator=(Counter&& other) noexcept {
        if (this != &other) {
            close_fd();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    ~Counter() { close_fd(); }

    // Returns an invalid counter when the event cannot be opened.
    static Counter try_open(Event event) {
        struct perf_event_attr attr {};
        attr.size = sizeof(attr);
        attr.type = PERF_TYPE_HARDWARE;
        attr.disabled = 1;
        // Count userspace only: works under stricter perf_event_paranoid
        // settings and keeps numbers attributable to the measured code.
        attr.exclude_kernel = 1;
        attr.exclude_hv = 1;
        switch (event) {
            case Event::CpuCycles:
                attr.config = PERF_COUNT_HW_CPU_CYCLES;
                break;
            case Event::Instructions:
                attr.config = PERF_COUNT_HW_INSTRUCTIONS;
                break;
            case Event::CacheMisses:
                attr.config = PERF_COUNT_HW_CACHE_MISSES;
                break;
            case Event::BranchMisses:
                attr.config = PERF_COUNT_HW_BRANCH_MISSES;
                break;
        }

        const int fd = static_cast<int>(
            syscall(SYS_perf_event_open, &attr, /*pid=*/0, /*cpu=*/-1, /*group_fd=*/-1,
                    /*flags=*/0));
        Counter counter;
        counter.fd_ = fd;
        return counter;
    }

    bool valid() const { return fd_ >= 0; }

    void reset_start() {
        if (fd_ < 0) {
            return;
        }
        ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
    }

    // Stops counting and returns the value accumulated since reset_start().
    std::uint64_t stop_read() {
        if (fd_ < 0) {
            return 0;
        }
        ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
        std::uint64_t value = 0;
        if (::read(fd_, &value, sizeof(value)) != sizeof(value)) {
            return 0;
        }
        return value;
    }

private:
    void close_fd() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    int fd_ = -1;
};

// Snapshot of the four teaching counters over one measured region.
struct Sample {
    bool available = false;
    std::uint64_t cpu_cycles = 0;
    std::uint64_t instructions = 0;
    std::uint64_t cache_misses = 0;
    std::uint64_t branch_misses = 0;

    double ipc() const {
        return cpu_cycles > 0 ? static_cast<double>(instructions) /
                                    static_cast<double>(cpu_cycles)
                              : 0.0;
    }
};

// Counts all four events around `body`. When any counter cannot be opened
// (WSL2/VM/paranoid), returns {available=false} and still runs `body` once.
template <typename Fn>
Sample measure(Fn&& body) {
    Counter cycles = Counter::try_open(Event::CpuCycles);
    Counter instructions = Counter::try_open(Event::Instructions);
    Counter cache_misses = Counter::try_open(Event::CacheMisses);
    Counter branch_misses = Counter::try_open(Event::BranchMisses);

    const bool available = cycles.valid() && instructions.valid() && cache_misses.valid() &&
                           branch_misses.valid();

    Sample sample;
    sample.available = available;

    if (available) {
        cycles.reset_start();
        instructions.reset_start();
        cache_misses.reset_start();
        branch_misses.reset_start();
    }

    body();

    if (available) {
        sample.cpu_cycles = cycles.stop_read();
        sample.instructions = instructions.stop_read();
        sample.cache_misses = cache_misses.stop_read();
        sample.branch_misses = branch_misses.stop_read();
    }
    return sample;
}

// Cheap probe: can this process open a cycles counter at all?
inline bool available() { return Counter::try_open(Event::CpuCycles).valid(); }

inline void print_unavailable_notice(const char* what) {
    std::printf("%s: PMU counters unavailable here (WSL2/VM or perf_event_paranoid).\n"
                "  The code still runs; on bare-metal Linux it would also report\n"
                "  cycles/IPC/cache and branch misses.\n",
                what);
}

}  // namespace hpc::pmu

#endif  // __linux__
