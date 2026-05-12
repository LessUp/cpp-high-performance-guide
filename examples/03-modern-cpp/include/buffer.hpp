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
    Buffer() : data_(nullptr), size_(0) {}

    explicit Buffer(size_t size) : data_(new char[size]), size_(size) {
        std::memset(data_, 0, size_);
    }

    ~Buffer() { delete[] data_; }

    Buffer(const Buffer& other) : data_(nullptr), size_(other.size_) {
        if (size_ > 0) {
            data_ = new char[size_];
            std::memcpy(data_, other.data_, size_);
        }
        ++copy_count_;
    }

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

    Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        ++move_count_;
    }

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

inline size_t Buffer::copy_count_ = 0;
inline size_t Buffer::move_count_ = 0;

}  // namespace hpc::move_semantics
