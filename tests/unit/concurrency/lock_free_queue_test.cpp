/**
 * @file lock_free_queue_test.cpp
 * @brief Unit tests for lock-free SPSC and MPMC queues
 *
 * These tests verify the correctness of lock-free queue implementations.
 * The queue implementations are copied from examples/05-concurrency/src/lock_free_queue.cpp
 * for testing purposes.
 */

#include <gtest/gtest.h>

#include <atomic>
#include <optional>
#include <thread>
#include <vector>

#include "hpc/core.hpp"

namespace hpc::concurrency::test {

//------------------------------------------------------------------------------
// SPSCQueue implementation for testing
//------------------------------------------------------------------------------

template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

public:
    SPSCQueue() : head_(0), tail_(0) {}

    bool push(const T& value) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & MASK;

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[current_tail].emplace(value);
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool push(T&& value) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & MASK;

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[current_tail].emplace(std::move(value));
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        const size_t current_head = head_.load(std::memory_order_relaxed);

        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt;
        }

        T value = std::move(*buffer_[current_head]);
        buffer_[current_head].reset();
        head_.store((current_head + 1) & MASK, std::memory_order_release);
        return value;
    }

    bool empty() const {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }

    size_t size() const {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail - head) & MASK;
    }

    constexpr size_t capacity() const { return Capacity - 1; }

private:
    static constexpr size_t MASK = Capacity - 1;

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_;
    alignas(CACHE_LINE_SIZE) std::optional<T> buffer_[Capacity];
};

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

    // Fill and drain multiple times to test wrap-around
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

    // Verify correctness
    EXPECT_EQ(received.size(), NUM_ITEMS);
    for (int i = 0; i < NUM_ITEMS; ++i) {
        EXPECT_EQ(received[i], i);
    }
}

TEST(SPSCQueueTest, CapacityReport) {
    SPSCQueue<int, 16> queue;
    EXPECT_EQ(queue.capacity(), 15);  // One slot is always empty
}

}  // namespace hpc::concurrency::test
