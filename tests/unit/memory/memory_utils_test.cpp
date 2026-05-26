/**
 * @file memory_utils_test.cpp
 * @brief Unit tests for memory_utils.hpp
 */

#include "memory_utils.hpp"

#include <gtest/gtest.h>

using hpc::core::CACHE_LINE_SIZE;
using hpc::core::PAGE_SIZE;

namespace hpc::memory::test {

namespace {

struct TrackedObject {
    inline static int constructions = 0;
    inline static int destructions = 0;

    TrackedObject() { ++constructions; }
    ~TrackedObject() { ++destructions; }
};

}  // namespace

// ---------------------------------------------------------------------------
// AlignedAllocator tests
// ---------------------------------------------------------------------------

TEST(AlignedAllocatorTest, VectorAllocatesDeallocates) {
    aligned_vector<float> vec(1024, 1.0f);
    EXPECT_EQ(vec.size(), 1024u);
    EXPECT_EQ(vec[0], 1.0f);
}

TEST(AlignedAllocatorTest, AlignmentIsCacheLine) {
    aligned_vector<float> vec(256);
    auto addr = reinterpret_cast<uintptr_t>(vec.data());
    EXPECT_EQ(addr % CACHE_LINE_SIZE, 0u);
}

// ---------------------------------------------------------------------------
// CacheLinePadded tests
// ---------------------------------------------------------------------------

TEST(CacheLinePaddedTest, SizeIsAtLeastCacheLine) {
    EXPECT_GE(sizeof(CacheLinePadded<int>), CACHE_LINE_SIZE);
}

TEST(CacheLinePaddedTest, AlignmentIsCacheLine) {
    EXPECT_EQ(alignof(CacheLinePadded<int>), CACHE_LINE_SIZE);
}

TEST(CacheLinePaddedTest, ValueAccessWorks) {
    CacheLinePadded<int> padded(42);
    EXPECT_EQ(padded.value, 42);
}

// ---------------------------------------------------------------------------
// aligned_alloc / aligned_free tests
// ---------------------------------------------------------------------------

TEST(AlignedAllocTest, AllocAndFree) {
    void* ptr = aligned_alloc(1024, 64);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % 64, 0u);
    aligned_free(ptr);
}

TEST(AlignedAllocTest, MakeAlignedUniquePtr) {
    auto ptr = make_aligned<float>(256, CACHE_LINE_SIZE);
    ASSERT_NE(ptr.get(), nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr.get()) % CACHE_LINE_SIZE, 0u);
}

TEST(AlignedAllocTest, MakeAlignedConstructsAndDestroysObjects) {
    TrackedObject::constructions = 0;
    TrackedObject::destructions = 0;

    {
        auto ptr = make_aligned<TrackedObject>(3, CACHE_LINE_SIZE);
        ASSERT_NE(ptr.get(), nullptr);
        EXPECT_EQ(TrackedObject::constructions, 3);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr.get()) % CACHE_LINE_SIZE, 0u);
    }

    EXPECT_EQ(TrackedObject::destructions, 3);
}

// ---------------------------------------------------------------------------
// Constants tests
// ---------------------------------------------------------------------------

TEST(ConstantsTest, CacheLineSizeIsReasonable) {
    EXPECT_EQ(CACHE_LINE_SIZE, 64u);
}

TEST(ConstantsTest, PageSizeIsReasonable) {
    EXPECT_EQ(PAGE_SIZE, 4096u);
}

}  // namespace hpc::memory::test
