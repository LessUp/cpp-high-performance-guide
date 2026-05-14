/**
 * @file buffer.hpp
 * @brief Buffer class demonstrating move semantics
 *
 * This class owns a dynamically allocated buffer and demonstrates
 * the difference between copying (expensive) and moving (cheap).
 *
 * Key concepts:
 * - Move constructor vs copy constructor
 * - std::move and rvalue references
 * - RAII for resource management
 */

#pragma once

#include <cstring>
#include <utility>

#include "instrumentation.hpp"

namespace hpc::move_semantics {

/**
 * @brief A buffer class with explicit copy and move operations
 *
 * This class owns a dynamically allocated buffer and demonstrates
 * the difference between copying (expensive) and moving (cheap).
 *
 * @note Copy operations are expensive (O(n) memory copy)
 * @note Move operations are cheap (O(1) pointer transfer)
 */
class Buffer {
public:
    Buffer() : data_(nullptr), size_(0), metrics_(nullptr) {}

    explicit Buffer(size_t size, instrumentation::OperationMetrics* metrics = nullptr)
        : data_(new char[size]), size_(size), metrics_(metrics) {
        std::memset(data_, 0, size_);
    }

    ~Buffer() { delete[] data_; }

    Buffer(const Buffer& other, instrumentation::OperationMetrics* metrics = nullptr)
        : data_(nullptr), size_(other.size_), metrics_(metrics ? metrics : other.metrics_) {
        if (size_ > 0) {
            data_ = new char[size_];
            std::memcpy(data_, other.data_, size_);
        }
        notify_copy();
    }

    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            metrics_ = other.metrics_;
            if (size_ > 0) {
                data_ = new char[size_];
                std::memcpy(data_, other.data_, size_);
            } else {
                data_ = nullptr;
            }
            notify_copy();
        }
        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_), metrics_(other.metrics_) {
        other.data_ = nullptr;
        other.size_ = 0;
        notify_move();
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            metrics_ = other.metrics_;
            other.data_ = nullptr;
            other.size_ = 0;
            notify_move();
        }
        return *this;
    }

    size_t size() const { return size_; }
    char* data() { return data_; }
    const char* data() const { return data_; }

private:
    char* data_;
    size_t size_;
    instrumentation::OperationMetrics* metrics_;

    void notify_copy() const {
        if (metrics_)
            metrics_->record_copy();
    }
    void notify_move() const {
        if (metrics_)
            metrics_->record_move();
    }
};

//------------------------------------------------------------------------------
// Functions demonstrating copy vs move
//------------------------------------------------------------------------------

/**
 * @brief Process buffer by copy (expensive)
 */
inline void process_by_copy(Buffer buf) {
    // Do something with buf
    volatile char c = buf.data()[0];
    (void)c;
}

/**
 * @brief Process buffer by const reference (no copy)
 */
inline void process_by_ref(const Buffer& buf) {
    // Do something with buf
    volatile char c = buf.data()[0];
    (void)c;
}

}  // namespace hpc::move_semantics
