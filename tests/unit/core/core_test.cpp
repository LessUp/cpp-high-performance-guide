/**
 * @file core_test.cpp
 * @brief Unit tests for core.hpp platform detection utilities
 *
 * Tests for:
 *   - cache_line_size() and CACHE_LINE_SIZE constant
 *   - page_size() and PAGE_SIZE constant
 *   - hardware_concurrency()
 */

#include <gtest/gtest.h>

#include <cmath>
#include <hpc/core.hpp>

namespace hpc::core::test {

//------------------------------------------------------------------------------
// Cache Line Size Tests
//------------------------------------------------------------------------------

TEST(CacheLineSizeTest, RuntimeReturnsPowerOfTwo) {
    std::size_t size = cache_line_size();
    // Check if power of two
    EXPECT_GT(size, 0u);
    EXPECT_EQ(size & (size - 1), 0u) << "cache_line_size() = " << size << " is not a power of two";
}

TEST(CacheLineSizeTest, RuntimeInReasonableRange) {
    std::size_t size = cache_line_size();
    // Cache line sizes typically range from 16 to 256 bytes
    EXPECT_GE(size, 16u);
    EXPECT_LE(size, 256u);
}

TEST(CacheLineSizeTest, CompileTimeIsPowerOfTwo) {
    constexpr std::size_t size = CACHE_LINE_SIZE;
    EXPECT_GT(size, 0u);
    EXPECT_EQ(size & (size - 1), 0u) << "CACHE_LINE_SIZE = " << size << " is not a power of two";
}

TEST(CacheLineSizeTest, CompileTimeInReasonableRange) {
    constexpr std::size_t size = CACHE_LINE_SIZE;
    EXPECT_GE(size, 16u);
    EXPECT_LE(size, 256u);
}

TEST(CacheLineSizeTest, RuntimeEqualsCompileTimeOrReasonableDefault) {
    // Runtime detection may differ from compile-time constant on some platforms
    // Both should be valid cache line sizes
    std::size_t runtime = cache_line_size();
    constexpr std::size_t compile_time = CACHE_LINE_SIZE;

    // Either they match, or runtime is a valid alternative (e.g., 128 on Apple M1)
    bool is_valid =
        (runtime == compile_time) || (runtime == 32) || (runtime == 64) || (runtime == 128);
    EXPECT_TRUE(is_valid) << "runtime=" << runtime << ", compile_time=" << compile_time;
}

//------------------------------------------------------------------------------
// Page Size Tests
//------------------------------------------------------------------------------

TEST(PageSizeTest, RuntimeReturnsPowerOfTwo) {
    std::size_t size = page_size();
    EXPECT_GT(size, 0u);
    EXPECT_EQ(size & (size - 1), 0u) << "page_size() = " << size << " is not a power of two";
}

TEST(PageSizeTest, RuntimeInReasonableRange) {
    std::size_t size = page_size();
    // Page sizes typically range from 1024 to 65536 bytes
    EXPECT_GE(size, 1024u);
    EXPECT_LE(size, 65536u);
}

TEST(PageSizeTest, CompileTimeIsPowerOfTwo) {
    constexpr std::size_t size = PAGE_SIZE;
    EXPECT_GT(size, 0u);
    EXPECT_EQ(size & (size - 1), 0u) << "PAGE_SIZE = " << size << " is not a power of two";
}

TEST(PageSizeTest, CompileTimeInReasonableRange) {
    constexpr std::size_t size = PAGE_SIZE;
    EXPECT_GE(size, 1024u);
    EXPECT_LE(size, 65536u);
}

TEST(PageSizeTest, RuntimeAtLeastCompileTime) {
    // Runtime page size should be at least the compile-time default
    EXPECT_GE(page_size(), PAGE_SIZE);
}

//------------------------------------------------------------------------------
// Hardware Concurrency Tests
//------------------------------------------------------------------------------

TEST(HardwareConcurrencyTest, ReturnsPositive) {
    unsigned int n = hardware_concurrency();
    EXPECT_GE(n, 1u);
}

TEST(HardwareConcurrencyTest, ReasonableMaximum) {
    unsigned int n = hardware_concurrency();
    // Modern systems rarely have more than 1024 hardware threads
    EXPECT_LE(n, 1024u);
}

TEST(HardwareConcurrencyTest, ConsistentWithStdThread) {
    // Should match std::thread::hardware_concurrency() or return 1 as fallback
    unsigned int std_concurrency = std::thread::hardware_concurrency();
    unsigned int our_concurrency = hardware_concurrency();

    if (std_concurrency > 0) {
        EXPECT_EQ(our_concurrency, std_concurrency);
    } else {
        EXPECT_EQ(our_concurrency, 1u);
    }
}

//------------------------------------------------------------------------------
// Integration Tests
//------------------------------------------------------------------------------

TEST(CoreIntegrationTest, CacheLineSizeDividesPageSize) {
    // Page size should be a multiple of cache line size
    std::size_t page = page_size();
    std::size_t cache_line = cache_line_size();

    EXPECT_EQ(page % cache_line, 0u)
        << "page_size=" << page << " is not a multiple of cache_line_size=" << cache_line;
}

TEST(CoreIntegrationTest, ConstantsAreConsistent) {
    // Compile-time constants should be valid defaults
    EXPECT_GE(CACHE_LINE_SIZE, 16u);
    EXPECT_LE(CACHE_LINE_SIZE, 256u);
    EXPECT_GE(PAGE_SIZE, 1024u);
    EXPECT_LE(PAGE_SIZE, 65536u);
}

}  // namespace hpc::core::test
