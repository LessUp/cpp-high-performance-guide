#pragma once

/**
 * @file arena.hpp
 * @brief Monotonic (bump) allocator for phase-based allocation patterns.
 *
 * Thousands of small new/delete calls pay per-object malloc overhead and
 * fragment the heap. When the objects all die together (parse -> build ->
 * emit phases, per-frame scratch data, request-scoped temporaries), an arena
 * replaces every allocation with a pointer bump and every deallocation with
 * one reset().
 *
 * Constraints (by design):
 * - no per-object free; memory is reclaimed only by reset()/destruction;
 * - reset() does NOT run destructors — use it for trivially destructible
 *   types or phase data whose cleanup is handled elsewhere.
 */

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <utility>

namespace hpc::mem {

class Arena {
public:
    explicit Arena(std::size_t capacity) : capacity_(capacity) {
        base_ = static_cast<std::byte*>(std::malloc(capacity));
        if (base_ == nullptr) {
            throw std::bad_alloc();
        }
    }

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    ~Arena() { std::free(base_); }

    // Returns nullptr when the request does not fit; callers decide whether
    // to grow, fall back, or fail.
    void* allocate(std::size_t bytes, std::size_t align = alignof(std::max_align_t)) {
        const std::uintptr_t base_addr = reinterpret_cast<std::uintptr_t>(base_);
        const std::uintptr_t current = base_addr + offset_;
        const std::uintptr_t aligned = (current + align - 1) & ~(align - 1);
        const std::size_t new_offset = (aligned - base_addr) + bytes;
        if (new_offset > capacity_) {
            return nullptr;
        }
        offset_ = new_offset;
        return reinterpret_cast<void*>(aligned);
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* slot = allocate(sizeof(T), alignof(T));
        if (slot == nullptr) {
            return nullptr;
        }
        return new (slot) T(std::forward<Args>(args)...);
    }

    void reset() { offset_ = 0; }

    std::size_t used() const { return offset_; }
    std::size_t capacity() const { return capacity_; }

private:
    std::byte* base_ = nullptr;
    std::size_t capacity_ = 0;
    std::size_t offset_ = 0;
};

}  // namespace hpc::mem
