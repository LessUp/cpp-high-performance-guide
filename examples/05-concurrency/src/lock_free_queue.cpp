/**
 * @file lock_free_queue.cpp
 * @brief Lock-free Single-Producer Single-Consumer (SPSC) queue implementation
 * 
 * This example demonstrates:
 * 1. Lock-free programming principles
 * 2. Memory ordering for producer-consumer patterns
 * 3. Cache-friendly queue design
 */

#include "../include/concurrency_utils.hpp"
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <cassert>

namespace hpc::concurrency {

/**
 * Lock-free SPSC (Single-Producer Single-Consumer) Queue
 * 
 * This is a bounded, lock-free queue that supports exactly one producer
 * and one consumer thread. It uses a ring buffer with atomic head and tail
 * pointers.
 * 
 * Key design decisions:
 * 1. Power-of-2 capacity for fast modulo (bitwise AND)
 * 2. Separate cache lines for head and tail to avoid false sharing
 * 3. Acquire-release ordering for synchronization
 */
template<typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity >= 2, "Capacity must be at least 2");
    
public:
    SPSCQueue() : head_(0), tail_(0) {
        // Initialize buffer
        for (size_t i = 0; i < Capacity; ++i) {
            buffer_[i] = T{};
        }
    }
    
    /**
     * Push an element to the queue (producer only)
     * @return true if successful, false if queue is full
     */
    bool push(const T& value) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & MASK;
        
        // Check if queue is full
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;  // Queue is full
        }
        
        buffer_[current_tail] = value;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }
    
    /**
     * Push with move semantics
     */
    bool push(T&& value) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & MASK;
        
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;
        }
        
        buffer_[current_tail] = std::move(value);
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }
    
    /**
     * Pop an element from the queue (consumer only)
     * @return optional containing the value, or empty if queue is empty
     */
    std::optional<T> pop() {
        const size_t current_head = head_.load(std::memory_order_relaxed);
        
        // Check if queue is empty
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt;  // Queue is empty
        }
        
        T value = std::move(buffer_[current_head]);
        head_.store((current_head + 1) & MASK, std::memory_order_release);
        return value;
    }
    
    /**
     * Check if queue is empty (approximate, may be stale)
     */
    bool empty() const {
        return head_.load(std::memory_order_relaxed) == 
               tail_.load(std::memory_order_relaxed);
    }
    
    /**
     * Get approximate size (may be stale)
     */
    size_t size() const {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail - head) & MASK;
    }
    
    /**
     * Get capacity
     */
    constexpr size_t capacity() const {
        return Capacity - 1;  // One slot is always empty
    }

private:
    static constexpr size_t MASK = Capacity - 1;
    
    // Align head and tail to separate cache lines to avoid false sharing
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_;
    alignas(CACHE_LINE_SIZE) T buffer_[Capacity];
};

/**
 * Lock-free MPMC (Multi-Producer Multi-Consumer) Queue
 * 
 * A more complex queue that supports multiple producers and consumers.
 * Uses sequence numbers for coordination.
 */
template<typename T, size_t Capacity>
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
    
    bool push(const T& value) {
        Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        
        for (;;) {
            cell = &cells_[pos & MASK];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
            
            if (diff == 0) {
                // Cell is ready for writing
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1,
                                                        std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                // Queue is full
                return false;
            } else {
                // Another producer got here first, retry
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
        
        cell->data = value;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }
    
    std::optional<T> pop() {
        Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        
        for (;;) {
            cell = &cells_[pos & MASK];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            
            if (diff == 0) {
                // Cell is ready for reading
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                                                        std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                // Queue is empty
                return std::nullopt;
            } else {
                // Another consumer got here first, retry
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
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            while (!queue.push(i)) {
                // Queue full, spin
                std::this_thread::yield();
            }
        }
        producer_done.store(true, std::memory_order_release);
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
            if (auto value = queue.pop()) {
                received.push_back(*value);
            } else {
                std::this_thread::yield();
            }
        }
        // Drain remaining items
        while (auto value = queue.pop()) {
            received.push_back(*value);
        }
    });
    
    producer.join();
    consumer.join();
    
    // Verify FIFO ordering
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
    
    // Start producers
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
    
    // Start consumers
    for (int c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&]() {
            while (!done.load(std::memory_order_acquire)) {
                if (queue.pop()) {
                    items_consumed.fetch_add(1, std::memory_order_relaxed);
                } else {
                    std::this_thread::yield();
                }
            }
            // Drain remaining
            while (queue.pop()) {
                items_consumed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    
    // Wait for producers
    for (auto& t : producers) {
        t.join();
    }
    
    // Signal consumers to finish
    done.store(true, std::memory_order_release);
    
    // Wait for consumers
    for (auto& t : consumers) {
        t.join();
    }
    
    int expected = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    std::cout << "Producers: " << NUM_PRODUCERS << ", Consumers: " << NUM_CONSUMERS << std::endl;
    std::cout << "Items produced: " << items_produced.load() << std::endl;
    std::cout << "Items consumed: " << items_consumed.load() << std::endl;
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "All items accounted for: " 
              << (items_consumed.load() == expected ? "Yes" : "No") << std::endl;
}

void demonstrate_lock_free_queue() {
    demonstrate_spsc_queue();
    demonstrate_mpmc_queue();
}

} // namespace hpc::concurrency

#ifndef HPC_BENCHMARK_MODE
int main() {
    hpc::concurrency::demonstrate_lock_free_queue();
    return 0;
}
#endif
