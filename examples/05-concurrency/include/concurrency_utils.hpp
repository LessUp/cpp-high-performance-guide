#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>

namespace hpc::concurrency {

/// Get the number of hardware threads
inline unsigned int hardware_concurrency() {
    unsigned int n = std::thread::hardware_concurrency();
    return n > 0 ? n : 1;
}

/// Cache line size for alignment
constexpr size_t CACHE_LINE_SIZE = 64;

/// Aligned atomic counter to avoid false sharing
struct alignas(CACHE_LINE_SIZE) AlignedCounter {
    std::atomic<int64_t> value{0};
    
    void increment(std::memory_order order = std::memory_order_seq_cst) {
        value.fetch_add(1, order);
    }
    
    int64_t load(std::memory_order order = std::memory_order_seq_cst) const {
        return value.load(order);
    }
    
    void store(int64_t v, std::memory_order order = std::memory_order_seq_cst) {
        value.store(v, order);
    }
};

/// Unaligned atomic counter (may suffer from false sharing)
struct UnalignedCounter {
    std::atomic<int64_t> value{0};
    
    void increment(std::memory_order order = std::memory_order_seq_cst) {
        value.fetch_add(1, order);
    }
    
    int64_t load(std::memory_order order = std::memory_order_seq_cst) const {
        return value.load(order);
    }
    
    void store(int64_t v, std::memory_order order = std::memory_order_seq_cst) {
        value.store(v, order);
    }
};

/// Simple spin lock using atomic flag
class SpinLock {
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // Spin
            #if defined(__cpp_lib_atomic_flag_test)
            while (flag_.test(std::memory_order_relaxed)) {
                // Reduce cache line bouncing
            }
            #endif
        }
    }
    
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
    
    bool try_lock() {
        return !flag_.test_and_set(std::memory_order_acquire);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

/// RAII lock guard for SpinLock
class SpinLockGuard {
public:
    explicit SpinLockGuard(SpinLock& lock) : lock_(lock) {
        lock_.lock();
    }
    
    ~SpinLockGuard() {
        lock_.unlock();
    }
    
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;

private:
    SpinLock& lock_;
};

/// Run a function on multiple threads and measure time
template<typename Func>
double run_parallel(Func&& func, unsigned int num_threads) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(std::forward<Func>(func), i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

} // namespace hpc::concurrency
