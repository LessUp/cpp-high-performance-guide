/**
 * @file arena_test.cpp
 * @brief Unit tests for the canonical hpc::mem::Arena.
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <hpc/arena.hpp>

namespace hpc::mem::test {

TEST(ArenaTest, AllocatesSequentiallyAndTracksUsage) {
    Arena arena(1024);
    EXPECT_EQ(arena.used(), 0u);

    void* first = arena.allocate(100);
    ASSERT_NE(first, nullptr);
    EXPECT_GE(arena.used(), 100u);

    void* second = arena.allocate(100);
    ASSERT_NE(second, nullptr);
    EXPECT_GT(static_cast<std::byte*>(second), static_cast<std::byte*>(first));
}

TEST(ArenaTest, HonorsAlignmentRequests) {
    Arena arena(4096);
    for (const std::size_t align : {8u, 16u, 32u, 64u}) {
        void* p = arena.allocate(7, align);
        ASSERT_NE(p, nullptr);
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % align, 0u);
    }
}

TEST(ArenaTest, ReturnsNullWhenExhausted) {
    Arena arena(64);
    EXPECT_NE(arena.allocate(64), nullptr);
    EXPECT_EQ(arena.allocate(1), nullptr);

    arena.reset();
    EXPECT_EQ(arena.used(), 0u);
    EXPECT_NE(arena.allocate(64), nullptr);
}

TEST(ArenaTest, CreateConstructsObjects) {
    struct Point {
        int x;
        int y;
        Point(int a, int b) : x(a), y(b) {}
    };

    Arena arena(256);
    Point* p = arena.create<Point>(3, 4);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(p->x, 3);
    EXPECT_EQ(p->y, 4);
}

}  // namespace hpc::mem::test
