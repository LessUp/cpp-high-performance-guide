/**
 * @file lock_free_queue_test.cpp
 * @brief Unit tests for lock-free SPSC and MPMC queues
 *
 * These tests verify the correctness of lock-free queue implementations.
 * The queue implementations are now in lock_free_queue.hpp for reuse.
 */

#include "lock_free_queue.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <optional>
#include <thread>
#include <vector>

namespace hpc::concurrency::test {

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
    for (int i = 0; i < NUM_ITEMS; ++i) {
        EXPECT_EQ(received[i], i);
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
    EXPECT_EQ(items_produced.load(), expected);
    EXPECT_EQ(items_consumed.load(), expected);
}

}  // namespace hpc::concurrency::test
