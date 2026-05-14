/**
 * @file move_semantics.cpp
 * @brief Move semantics performance demonstration
 *
 * This example shows the performance benefits of move semantics
 * compared to copy operations for objects with dynamically allocated memory.
 *
 * Key concepts:
 * - Move constructor vs copy constructor
 * - std::move and rvalue references
 * - Return value optimization (RVO/NRVO)
 * - When to use std::move
 */

#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

#include "buffer.hpp"
#include "instrumentation.hpp"

namespace hpc::move_semantics {

using hpc::instrumentation::OperationMetrics;

//------------------------------------------------------------------------------
// Functions demonstrating copy vs move
// Note: process_by_copy and process_by_ref are defined in buffer.hpp
//------------------------------------------------------------------------------

/**
 * @brief Returns a buffer by value (may use RVO)
 */
Buffer create_buffer(size_t size, OperationMetrics* metrics = nullptr) {
    Buffer buf(size, metrics);
    return buf;  // NRVO may elide the copy/move
}

/**
 * @brief Process buffer by move (cheap)
 */
void process_by_move(Buffer&& buf) {
    Buffer local = std::move(buf);  // Move into local
    volatile char c = local.data()[0];
    (void)c;
}

//------------------------------------------------------------------------------
// Vector operations demonstrating move semantics
//------------------------------------------------------------------------------

void demonstrate_vector_push_back() {
    std::cout << "\n=== Vector push_back ===\n";

    constexpr size_t BUFFER_SIZE = 1024 * 1024;  // 1 MB
    constexpr int NUM_BUFFERS = 100;

    // Push by copy
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        std::vector<Buffer> vec;
        vec.reserve(NUM_BUFFERS);  // Prevent reallocation

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            Buffer buf(BUFFER_SIZE, &metrics);
            vec.push_back(buf);  // Copy
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "push_back (copy): " << ms << " ms" << " (copies: " << metrics.copy_count
                  << ", moves: " << metrics.move_count << ")\n";
    }

    // Push by move
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        std::vector<Buffer> vec;
        vec.reserve(NUM_BUFFERS);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            Buffer buf(BUFFER_SIZE, &metrics);
            vec.push_back(std::move(buf));  // Move
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "push_back (move): " << ms << " ms" << " (copies: " << metrics.copy_count
                  << ", moves: " << metrics.move_count << ")\n";
    }

    // emplace_back (construct in place)
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        std::vector<Buffer> vec;
        vec.reserve(NUM_BUFFERS);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            vec.emplace_back(BUFFER_SIZE, &metrics);  // Construct in place
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "emplace_back:     " << ms << " ms" << " (copies: " << metrics.copy_count
                  << ", moves: " << metrics.move_count << ")\n";
    }
}

void demonstrate_function_calls() {
    std::cout << "\n=== Function Call Patterns ===\n";

    constexpr size_t BUFFER_SIZE = 1024 * 1024;  // 1 MB
    constexpr int ITERATIONS = 100;

    // By copy
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        Buffer buf(BUFFER_SIZE, &metrics);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            process_by_copy(buf);  // Copies each time
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "By copy:      " << ms << " ms" << " (copies: " << metrics.copy_count << ")\n";
    }

    // By reference
    {
        OperationMetrics metrics;
        OperationMetrics::Scope scope(metrics);
        Buffer buf(BUFFER_SIZE, &metrics);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            process_by_ref(buf);  // No copy
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "By reference: " << ms << " ms" << " (copies: " << metrics.copy_count << ")\n";
    }
}

void demonstrate_return_value() {
    std::cout << "\n=== Return Value Optimization ===\n";

    constexpr size_t BUFFER_SIZE = 1024 * 1024;
    constexpr int ITERATIONS = 100;

    OperationMetrics metrics;
    OperationMetrics::Scope scope(metrics);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        Buffer buf = create_buffer(BUFFER_SIZE, &metrics);
        volatile char c = buf.data()[0];
        (void)c;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Return by value: " << ms << " ms" << " (copies: " << metrics.copy_count
              << ", moves: " << metrics.move_count << ")\n";
    std::cout << "Note: With RVO/NRVO, copies and moves should be 0\n";
}

}  // namespace hpc::move_semantics

#ifndef HPC_TEST_MODE
int main() {
    std::cout << "=== Move Semantics Performance Demo ===\n";

    hpc::move_semantics::demonstrate_vector_push_back();
    hpc::move_semantics::demonstrate_function_calls();
    hpc::move_semantics::demonstrate_return_value();

    std::cout << "\nKey takeaways:\n";
    std::cout << "1. Use std::move when you no longer need the source object\n";
    std::cout << "2. Use emplace_back instead of push_back when possible\n";
    std::cout << "3. Pass large objects by const reference when not transferring ownership\n";
    std::cout << "4. Return by value - RVO/NRVO will optimize it\n";

    return 0;
}
#endif
