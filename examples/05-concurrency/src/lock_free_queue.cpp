/**
 * @file lock_free_queue.cpp
 * @brief Lock-free queue demonstration and examples
 *
 * This example demonstrates:
 * 1. Lock-free programming principles
 * 2. Memory ordering for producer-consumer patterns
 * 3. Cache-friendly queue design
 */

#include <cassert>
#include <hpc/lock_free_queue.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace hpc::concurrency {

// ============================================================================
// Demo and verification
// ============================================================================

void demonstrate_spsc_queue() {
    std::cout << "=== SPSC Queue Demo ===" << std::endl;

    SPSCQueue<int, 1024> queue;
    constexpr int NUM_ITEMS = 100000;

    std::atomic<bool> producer_done{false};
    std::vector<int> received;
    received.reserve(NUM_ITEMS);

    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
        }
        producer_done.store(true, std::memory_order_release);
    });

    std::thread consumer([&]() {
        while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
            if (auto value = queue.pop()) {
                received.push_back(*value);
            } else {
                std::this_thread::yield();
            }
        }
        while (auto value = queue.pop()) {
            received.push_back(*value);
        }
    });

    producer.join();
    consumer.join();

    bool correct = (received.size() == NUM_ITEMS);
    for (size_t i = 0; i < received.size() && correct; ++i) {
        if (received[i] != static_cast<int>(i)) {
            correct = false;
        }
    }

    std::cout << "Items sent: " << NUM_ITEMS << std::endl;
    std::cout << "Items received: " << received.size() << std::endl;
    std::cout << "FIFO order preserved: " << (correct ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

void demonstrate_mpmc_queue() {
    std::cout << "=== MPMC Queue Demo ===" << std::endl;

    MPMCQueue<int, 1024> queue;
    constexpr int NUM_PRODUCERS = 4;
    constexpr int NUM_CONSUMERS = 4;
    constexpr int ITEMS_PER_PRODUCER = 10000;

    std::atomic<int> items_produced{0};
    std::atomic<int> items_consumed{0};
    std::atomic<bool> done{false};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                int value = p * ITEMS_PER_PRODUCER + i;
                while (!queue.push(value)) {
                    std::this_thread::yield();
                }
                items_produced.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (int c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&]() {
            while (!done.load(std::memory_order_acquire)) {
                if (queue.pop()) {
                    items_consumed.fetch_add(1, std::memory_order_relaxed);
                } else {
                    std::this_thread::yield();
                }
            }
            while (queue.pop()) {
                items_consumed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : producers) {
        t.join();
    }

    done.store(true, std::memory_order_release);

    for (auto& t : consumers) {
        t.join();
    }

    int expected = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    std::cout << "Producers: " << NUM_PRODUCERS << ", Consumers: " << NUM_CONSUMERS << std::endl;
    std::cout << "Items produced: " << items_produced.load() << std::endl;
    std::cout << "Items consumed: " << items_consumed.load() << std::endl;
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "All items accounted for: " << (items_consumed.load() == expected ? "Yes" : "No")
              << std::endl;
}

void demonstrate_lock_free_queue() {
    demonstrate_spsc_queue();
    demonstrate_mpmc_queue();
}

}  // namespace hpc::concurrency

#ifndef HPC_BENCHMARK_MODE
int main() {
    hpc::concurrency::demonstrate_lock_free_queue();
    return 0;
}
#endif
