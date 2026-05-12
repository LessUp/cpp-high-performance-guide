/**
 * @file lock_free_queue.hpp
 * @brief Lock-free Single-Producer Single-Consumer (SPSC) and Multi-Producer Multi-Consumer (MPMC)
 * queue implementations
 *
 * This header provides lock-free queue implementations:
 * 1. SPSCQueue - for single-producer single-consumer scenarios
 * 2. MPMCQueue - for multi-producer multi-consumer scenarios
 *
 * Key design decisions:
 * 1. Power-of-2 capacity for fast modulo (bitwise AND)
 * 2. Separate cache lines for head and tail to avoid false sharing
 * 3. Acquire-release ordering for synchronization
 * 4. Uses std::optional for buffer storage to support non-default-constructible types
 */

#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include "concurrency_utils.hpp"

namespace hpc::concurrency {

//------------------------------------------------------------------------------
// SPSCQueue - Lock-free Single-Producer Single-Consumer Queue
//------------------------------------------------------------------------------

/**
 * @brief Lock-free bounded queue for single-producer single-consumer scenarios
 *
 * This is a bounded, lock-free queue that supports exactly one producer
 * and one consumer thread. It uses a ring buffer with atomic head and tail
 * pointers.
 *
 * @tparam T Element type
 * @tparam Capacity Queue capacity (must be power of 2 and at least 2)
 *
 * @note Thread-safe for exactly one producer thread and one consumer thread
 * @note One slot is always empty, so actual capacity is Capacity - 1
 */
template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

public:
    SPSCQueue() : head_(0), tail_(0) {}

    /**
     * @brief Push an element to the queue (producer only)
     * @param value Element to push
     * @return true if successful, false if queue is full
     */
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

    /**
     * @brief Push with move semantics (producer only)
     */
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

    /**
     * @brief Pop an element from the queue (consumer only)
     * @return optional containing the value, or empty if queue is empty
     */
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

    /**
     * @brief Check if queue is empty (approximate, may be stale)
     */
    bool empty() const {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }

    /**
     * @brief Get approximate size (may be stale)
     */
    size_t size() const {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail - head) & MASK;
    }

    /**
     * @brief Get capacity
     */
    constexpr size_t capacity() const { return Capacity - 1; }

private:
    static constexpr size_t MASK = Capacity - 1;

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_;
    alignas(CACHE_LINE_SIZE) std::optional<T> buffer_[Capacity];
};

//------------------------------------------------------------------------------
// MPMCQueue - Lock-free Multi-Producer Multi-Consumer Queue
//------------------------------------------------------------------------------

/**
 * @brief Lock-free bounded queue for multi-producer multi-consumer scenarios
 *
 * A more complex queue that supports multiple producers and consumers.
 * Uses sequence numbers for coordination.
 *
 * @tparam T Element type
 * @tparam Capacity Queue capacity (must be power of 2)
 *
 * @note Thread-safe for multiple producer and consumer threads
 */
template <typename T, size_t Capacity>
class MPMCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

public:
    MPMCQueue() : enqueue_pos_(0), dequeue_pos_(0) {
        for (size_t i = 0; i < Capacity; ++i) {
            cells_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    /**
     * @brief Push an element to the queue (thread-safe)
     * @param value Element to push
     * @return true if successful, false if queue is full
     */
    bool push(const T& value) {
        Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        int backoff = 1;

        for (;;) {
            cell = &cells_[pos & MASK];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (diff == 0) {
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                for (int i = 0; i < backoff; ++i) {
                    std::this_thread::yield();
                }
                backoff = std::min(backoff * 2, 64);
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }

        cell->data = value;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    /**
     * @brief Pop an element from the queue (thread-safe)
     * @return optional containing the value, or empty if queue is empty
     */
    std::optional<T> pop() {
        Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        int backoff = 1;

        for (;;) {
            cell = &cells_[pos & MASK];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

            if (diff == 0) {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return std::nullopt;
            } else {
                for (int i = 0; i < backoff; ++i) {
                    std::this_thread::yield();
                }
                backoff = std::min(backoff * 2, 64);
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }

        T value = std::move(cell->data);
        cell->sequence.store(pos + Capacity, std::memory_order_release);
        return value;
    }

private:
    static constexpr size_t MASK = Capacity - 1;

    alignas(CACHE_LINE_SIZE) Cell cells_[Capacity];
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> enqueue_pos_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> dequeue_pos_;
};

}  // namespace hpc::concurrency
