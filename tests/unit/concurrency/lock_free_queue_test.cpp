/**
 * @file lock_free_queue_test.cpp
 * @brief Unit tests for lock-free SPSC and MPMC queues
 *
 * These tests verify the correctness of lock-free queue implementations.
 * The queue implementations are now in lock_free_queue.hpp for reuse.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <hpc/lock_free_queue.hpp>
#include <random>
#include <thread>
#include <vector>

namespace hpc::concurrency::test {

namespace {

struct MoveOnlyPayload {
    int value;

    explicit MoveOnlyPayload(int v) : value(v) {}

    MoveOnlyPayload() = delete;
    MoveOnlyPayload(const MoveOnlyPayload&) = delete;
    MoveOnlyPayload& operator=(const MoveOnlyPayload&) = delete;
    MoveOnlyPayload(MoveOnlyPayload&&) noexcept = default;
    MoveOnlyPayload& operator=(MoveOnlyPayload&&) noexcept = default;
};

}  // namespace

//------------------------------------------------------------------------------
// SPSCQueue Tests
//------------------------------------------------------------------------------

TEST(SPSCQueueTest, PushPopSingleElement) {
    SPSCQueue<int, 16> queue;
    EXPECT_TRUE(queue.push(42));

    auto value = queue.pop();
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, 42);
}

TEST(SPSCQueueTest, EmptyQueueReturnsNullopt) {
    SPSCQueue<int, 16> queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);

    auto value = queue.pop();
    EXPECT_FALSE(value.has_value());
}

TEST(SPSCQueueTest, FullQueueRejectsPush) {
    SPSCQueue<int, 4> queue;  // capacity = 3

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_TRUE(queue.push(3));
    EXPECT_FALSE(queue.push(4));  // Should fail - queue is full
}

TEST(SPSCQueueTest, FIFOOrder) {
    SPSCQueue<int, 64> queue;

    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(queue.push(i));
    }

    for (int i = 0; i < 10; ++i) {
        auto value = queue.pop();
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(*value, i);
    }
}

TEST(SPSCQueueTest, WrapAround) {
    SPSCQueue<int, 8> queue;  // capacity = 7

    for (int round = 0; round < 3; ++round) {
        for (int i = 0; i < 5; ++i) {
            EXPECT_TRUE(queue.push(round * 100 + i));
        }

        for (int i = 0; i < 5; ++i) {
            auto value = queue.pop();
            EXPECT_TRUE(value.has_value());
            EXPECT_EQ(*value, round * 100 + i);
        }
    }
}

TEST(SPSCQueueTest, ConcurrentProducerConsumer) {
    SPSCQueue<int, 1024> queue;
    constexpr int NUM_ITEMS = 10000;

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

    EXPECT_EQ(received.size(), NUM_ITEMS);
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITEMS); ++i) {
        EXPECT_EQ(received[i], static_cast<int>(i));
    }
}

TEST(SPSCQueueTest, CapacityReport) {
    SPSCQueue<int, 16> queue;
    EXPECT_EQ(queue.capacity(), 15);  // One slot is always empty
}

//------------------------------------------------------------------------------
// MPMCQueue Tests
//------------------------------------------------------------------------------

TEST(MPMCQueueTest, PushPopSingleElement) {
    MPMCQueue<int, 16> queue;
    EXPECT_TRUE(queue.push(42));

    auto value = queue.pop();
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, 42);
}

TEST(MPMCQueueTest, EmptyQueueReturnsNullopt) {
    MPMCQueue<int, 16> queue;

    auto value = queue.pop();
    EXPECT_FALSE(value.has_value());
}

TEST(MPMCQueueTest, FIFOOrder) {
    MPMCQueue<int, 64> queue;

    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(queue.push(i));
    }

    for (int i = 0; i < 10; ++i) {
        auto value = queue.pop();
        EXPECT_TRUE(value.has_value());
        EXPECT_EQ(*value, i);
    }
}

TEST(MPMCQueueTest, ConcurrentMultipleProducersConsumers) {
    MPMCQueue<int, 1024> queue;
    constexpr int NUM_PRODUCERS = 4;
    constexpr int NUM_CONSUMERS = 4;
    constexpr int ITEMS_PER_PRODUCER = 1000;
    constexpr int EXPECTED_TOTAL = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    std::atomic<bool> done{false};

    // Each consumer records the values it observes. After joining, we verify
    // that every value in [0, EXPECTED_TOTAL) was delivered EXACTLY ONCE.
    // Comparing counts alone cannot catch "one value delivered twice and
    // another lost" — the counts would still match.
    std::vector<std::vector<int>> consumed_by(static_cast<size_t>(NUM_CONSUMERS));

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                int value = p * ITEMS_PER_PRODUCER + i;
                while (!queue.push(value)) {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (int c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&, c]() {
            auto& received = consumed_by[static_cast<size_t>(c)];
            while (!done.load(std::memory_order_acquire)) {
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
    }

    for (auto& t : producers) {
        t.join();
    }

    done.store(true, std::memory_order_release);

    for (auto& t : consumers) {
        t.join();
    }

    // Merge per-consumer records and verify each value exactly once.
    std::vector<int> all;
    all.reserve(EXPECTED_TOTAL);
    for (auto& received : consumed_by) {
        all.insert(all.end(), received.begin(), received.end());
    }

    ASSERT_EQ(static_cast<int>(all.size()), EXPECTED_TOTAL);
    std::sort(all.begin(), all.end());
    for (int i = 0; i < EXPECTED_TOTAL; ++i) {
        if (all[static_cast<size_t>(i)] != i) {
            FAIL() << "Value " << i << " missing or duplicated (got " << all[static_cast<size_t>(i)]
                   << " at sorted index " << i << ")";
        }
    }
}

TEST(MPMCQueueTest, FullQueueRejectsPush) {
    MPMCQueue<int, 8> queue;

    // Fill to capacity; push must then fail rather than overwrite.
    // (Unlike SPSC, the sequence-based MPMC design uses all Capacity slots.)
    int pushed = 0;
    while (queue.push(pushed)) {
        ++pushed;
    }
    EXPECT_EQ(pushed, 8);
    EXPECT_FALSE(queue.push(999));

    // Freeing one slot makes push succeed again.
    auto value = queue.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, 0);
    EXPECT_TRUE(queue.push(pushed));  // pushes 8

    // Drain and verify FIFO order across the rejection boundary:
    // nothing was lost or duplicated while the queue was full.
    std::vector<int> drained;
    while (auto v = queue.pop()) {
        drained.push_back(*v);
    }
    ASSERT_EQ(static_cast<int>(drained.size()), 8);
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(drained[static_cast<size_t>(i)], i + 1);
    }
}

TEST(MPMCQueueTest, SupportsNonDefaultConstructiblePayloads) {
    MPMCQueue<MoveOnlyPayload, 16> queue;

    EXPECT_TRUE(queue.push(MoveOnlyPayload{7}));

    auto value = queue.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value->value, 7);
}

//------------------------------------------------------------------------------
// Stress Tests
//------------------------------------------------------------------------------

/**
 * @brief High-throughput SPSC stress test with 1M operations
 *
 * Validates queue correctness under sustained high load.
 */
TEST(SPSCQueueStressTest, HighThroughputOneMillionOperations) {
    SPSCQueue<int, 4096> queue;
    // Sanitizers add 5-15x runtime overhead; scale the workload down so the
    // stress test still exercises wraparound under ASan/TSan without timing out.
#if defined(HPC_SANITIZER_BUILD)
    constexpr int NUM_ITEMS = 50000;
#else
    constexpr int NUM_ITEMS = 1000000;
#endif

    std::atomic<bool> producer_done{false};
    std::vector<int> received;
    received.reserve(NUM_ITEMS);

    auto start = std::chrono::steady_clock::now();

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

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_EQ(received.size(), NUM_ITEMS);

    // Verify FIFO ordering (spot check first and last 100 elements)
    for (size_t i = 0; i < 100; ++i) {
        EXPECT_EQ(received[i], static_cast<int>(i));
        const size_t idx = static_cast<size_t>(NUM_ITEMS) - 100 + i;
        EXPECT_EQ(received[idx], static_cast<int>(idx));
    }

    // Report throughput (guard against division by zero)
    double throughput =
        duration_ms > 0 ? static_cast<double>(NUM_ITEMS) / static_cast<double>(duration_ms) * 1000
                        : 0.0;
    std::cout << "SPSC throughput: " << static_cast<int>(throughput) << " ops/sec (" << duration_ms
              << " ms for " << NUM_ITEMS << " items)" << std::endl;
}

/**
 * @brief MPMC stress test with higher load
 *
 * Validates MPMC queue correctness under high contention.
 */
TEST(MPMCQueueStressTest, HighContentionMultipleProducersConsumers) {
    MPMCQueue<int, 2048> queue;
    constexpr int NUM_PRODUCERS = 4;
    constexpr int NUM_CONSUMERS = 4;
#if defined(HPC_SANITIZER_BUILD)
    constexpr int ITEMS_PER_PRODUCER = 5000;
#else
    constexpr int ITEMS_PER_PRODUCER = 50000;
#endif
    constexpr int EXPECTED_TOTAL = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    std::atomic<int> items_produced{0};
    std::atomic<int> items_consumed{0};
    std::atomic<bool> done{false};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    auto start = std::chrono::steady_clock::now();

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

    // Per-consumer value records for element-level verification (see
    // ConcurrentMultipleProducersConsumers for rationale); the atomic counter
    // remains the progress signal for the deadline loop below.
    std::vector<std::vector<int>> consumed_by(NUM_CONSUMERS);

    for (int c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&, c]() {
            auto& received = consumed_by[static_cast<size_t>(c)];
            while (!done.load(std::memory_order_acquire)) {
                if (auto value = queue.pop()) {
                    received.push_back(*value);
                    items_consumed.fetch_add(1, std::memory_order_relaxed);
                } else {
                    std::this_thread::yield();
                }
            }
            // Drain remaining items
            while (auto value = queue.pop()) {
                received.push_back(*value);
                items_consumed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : producers) {
        t.join();
    }

    // Wait for all items to be consumed with timeout (relaxed under sanitizers,
    // where atomic instrumentation slows the contention loops substantially).
#if defined(HPC_SANITIZER_BUILD)
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
#else
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
#endif
    while (items_consumed.load() < EXPECTED_TOTAL) {
        if (std::chrono::steady_clock::now() > deadline) {
            done.store(true, std::memory_order_release);
            for (auto& t : consumers) {
                t.join();
            }
            FAIL() << "Timeout: consumed " << items_consumed.load() << "/" << EXPECTED_TOTAL
                   << " items";
        }
        std::this_thread::yield();
    }
    done.store(true, std::memory_order_release);

    for (auto& t : consumers) {
        t.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_EQ(items_produced.load(), EXPECTED_TOTAL);
    EXPECT_EQ(items_consumed.load(), EXPECTED_TOTAL);

    // Element-level check: every value in [0, EXPECTED_TOTAL) exactly once.
    std::vector<int> all;
    all.reserve(EXPECTED_TOTAL);
    for (auto& received : consumed_by) {
        all.insert(all.end(), received.begin(), received.end());
    }
    ASSERT_EQ(static_cast<int>(all.size()), EXPECTED_TOTAL);
    std::sort(all.begin(), all.end());
    for (int i = 0; i < EXPECTED_TOTAL; ++i) {
        if (all[static_cast<size_t>(i)] != i) {
            FAIL() << "Value " << i << " missing or duplicated under contention";
        }
    }

    // Report throughput (guard against division by zero)
    double throughput = duration_ms > 0 ? static_cast<double>(EXPECTED_TOTAL) /
                                              static_cast<double>(duration_ms) * 1000
                                        : 0.0;
    std::cout << "MPMC throughput: " << static_cast<int>(throughput) << " ops/sec (" << duration_ms
              << " ms for " << EXPECTED_TOTAL << " items)" << std::endl;
}

/**
 * @brief SPSC test with random delays to stress interleaving
 *
 * Random delays can expose race conditions that pure spin-waiting might miss.
 */
TEST(SPSCQueueStressTest, RandomInterleavingWithDelays) {
    SPSCQueue<int, 256> queue;
    constexpr int NUM_ITEMS = 10000;

    std::atomic<bool> producer_done{false};
    std::vector<int> received;
    received.reserve(NUM_ITEMS);

    std::mt19937 rng(42);  // Fixed seed for reproducibility
    std::uniform_int_distribution<int> delay_dist(0, 100);

    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
            // Random delay to create interleaving
            if (delay_dist(rng) < 10) {
                std::this_thread::sleep_for(std::chrono::microseconds(delay_dist(rng)));
            }
        }
        producer_done.store(true, std::memory_order_release);
    });

    std::thread consumer([&]() {
        std::mt19937 consumer_rng(123);  // Different seed
        while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
            if (auto value = queue.pop()) {
                received.push_back(*value);
                // Random delay
                if (delay_dist(consumer_rng) < 10) {
                    std::this_thread::sleep_for(
                        std::chrono::microseconds(delay_dist(consumer_rng)));
                }
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

    EXPECT_EQ(received.size(), NUM_ITEMS);

    // Verify FIFO ordering
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITEMS); ++i) {
        EXPECT_EQ(received[i], static_cast<int>(i)) << "FIFO violation at index " << i;
    }
}

/**
 * @brief SPSC test with non-trivial type (std::string)
 *
 * Validates that move semantics work correctly under stress.
 */
TEST(SPSCQueueStressTest, NonTrivialTypeStress) {
    SPSCQueue<std::string, 256> queue;
    constexpr int NUM_ITEMS = 5000;

    std::atomic<bool> producer_done{false};
    std::vector<std::string> received;
    received.reserve(NUM_ITEMS);

    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            std::string value = "item_" + std::to_string(i);
            while (!queue.push(std::move(value))) {
                std::this_thread::yield();
            }
        }
        producer_done.store(true, std::memory_order_release);
    });

    std::thread consumer([&]() {
        while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
            if (auto value = queue.pop()) {
                received.push_back(std::move(*value));
            } else {
                std::this_thread::yield();
            }
        }
        while (auto value = queue.pop()) {
            received.push_back(std::move(*value));
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(received.size(), NUM_ITEMS);

    // Verify correct strings
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITEMS); ++i) {
        EXPECT_EQ(received[i], "item_" + std::to_string(i));
    }
}

}  // namespace hpc::concurrency::test
