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
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

namespace hpc::move_semantics {

//------------------------------------------------------------------------------
// Buffer class demonstrating move semantics
//------------------------------------------------------------------------------

/**
 * @brief A buffer class with explicit copy and move operations
 * 
 * This class owns a dynamically allocated buffer and demonstrates
 * the difference between copying (expensive) and moving (cheap).
 */
class Buffer {
public:
    // Default constructor
    Buffer() : data_(nullptr), size_(0) {}
    
    // Constructor with size
    explicit Buffer(size_t size) : data_(new char[size]), size_(size) {
        std::memset(data_, 0, size_);
    }
    
    // Destructor
    ~Buffer() {
        delete[] data_;
    }
    
    // Copy constructor - EXPENSIVE
    Buffer(const Buffer& other) : data_(nullptr), size_(other.size_) {
        if (size_ > 0) {
            data_ = new char[size_];
            std::memcpy(data_, other.data_, size_);
        }
        ++copy_count_;
    }
    
    // Copy assignment - EXPENSIVE
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            if (size_ > 0) {
                data_ = new char[size_];
                std::memcpy(data_, other.data_, size_);
            } else {
                data_ = nullptr;
            }
            ++copy_count_;
        }
        return *this;
    }
    
    // Move constructor - CHEAP
    Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        ++move_count_;
    }
    
    // Move assignment - CHEAP
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
            ++move_count_;
        }
        return *this;
    }
    
    size_t size() const { return size_; }
    char* data() { return data_; }
    const char* data() const { return data_; }
    
    // Statistics
    static size_t copy_count_;
    static size_t move_count_;
    
    static void reset_counts() {
        copy_count_ = 0;
        move_count_ = 0;
    }
    
private:
    char* data_;
    size_t size_;
};

size_t Buffer::copy_count_ = 0;
size_t Buffer::move_count_ = 0;

//------------------------------------------------------------------------------
// Functions demonstrating copy vs move
//------------------------------------------------------------------------------

/**
 * @brief Returns a buffer by value (may use RVO)
 */
Buffer create_buffer(size_t size) {
    Buffer buf(size);
    return buf;  // NRVO may elide the copy/move
}

/**
 * @brief Process buffer by copy (expensive)
 */
void process_by_copy(Buffer buf) {
    // Do something with buf
    volatile char c = buf.data()[0];
    (void)c;
}

/**
 * @brief Process buffer by const reference (no copy)
 */
void process_by_ref(const Buffer& buf) {
    // Do something with buf
    volatile char c = buf.data()[0];
    (void)c;
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
        Buffer::reset_counts();
        std::vector<Buffer> vec;
        vec.reserve(NUM_BUFFERS);  // Prevent reallocation
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            Buffer buf(BUFFER_SIZE);
            vec.push_back(buf);  // Copy
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "push_back (copy): " << ms << " ms"
                  << " (copies: " << Buffer::copy_count_ 
                  << ", moves: " << Buffer::move_count_ << ")\n";
    }
    
    // Push by move
    {
        Buffer::reset_counts();
        std::vector<Buffer> vec;
        vec.reserve(NUM_BUFFERS);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            Buffer buf(BUFFER_SIZE);
            vec.push_back(std::move(buf));  // Move
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "push_back (move): " << ms << " ms"
                  << " (copies: " << Buffer::copy_count_ 
                  << ", moves: " << Buffer::move_count_ << ")\n";
    }
    
    // emplace_back (construct in place)
    {
        Buffer::reset_counts();
        std::vector<Buffer> vec;
        vec.reserve(NUM_BUFFERS);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            vec.emplace_back(BUFFER_SIZE);  // Construct in place
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "emplace_back:     " << ms << " ms"
                  << " (copies: " << Buffer::copy_count_ 
                  << ", moves: " << Buffer::move_count_ << ")\n";
    }
}

void demonstrate_function_calls() {
    std::cout << "\n=== Function Call Patterns ===\n";
    
    constexpr size_t BUFFER_SIZE = 1024 * 1024;  // 1 MB
    constexpr int ITERATIONS = 100;
    
    // By copy
    {
        Buffer::reset_counts();
        Buffer buf(BUFFER_SIZE);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            process_by_copy(buf);  // Copies each time
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "By copy:      " << ms << " ms"
                  << " (copies: " << Buffer::copy_count_ << ")\n";
    }
    
    // By reference
    {
        Buffer::reset_counts();
        Buffer buf(BUFFER_SIZE);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            process_by_ref(buf);  // No copy
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "By reference: " << ms << " ms"
                  << " (copies: " << Buffer::copy_count_ << ")\n";
    }
}

void demonstrate_return_value() {
    std::cout << "\n=== Return Value Optimization ===\n";
    
    constexpr size_t BUFFER_SIZE = 1024 * 1024;
    constexpr int ITERATIONS = 100;
    
    Buffer::reset_counts();
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        Buffer buf = create_buffer(BUFFER_SIZE);
        volatile char c = buf.data()[0];
        (void)c;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Return by value: " << ms << " ms"
              << " (copies: " << Buffer::copy_count_ 
              << ", moves: " << Buffer::move_count_ << ")\n";
    std::cout << "Note: With RVO/NRVO, copies and moves should be 0\n";
}

} // namespace hpc::move_semantics

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
