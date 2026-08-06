/**
 * @file io_utils_test.cpp
 * @brief Unit tests for the canonical hpc::io module (Linux only).
 */

#include <gtest/gtest.h>

#if defined(__linux__)

#include <fcntl.h>
#include <unistd.h>

#include <hpc/io_utils.hpp>
#include <vector>

namespace hpc::io::test {

namespace {

// Mirrors make_temp_file's pattern so tests can verify content, not just size.
unsigned char expected_byte(std::size_t pos) {
    return static_cast<unsigned char>(pos & 0xFF);
}

}  // namespace

TEST(IoUtilsTest, MakeTempFileHasSizeAndPattern) {
    constexpr std::size_t kSize = 100000;  // not a multiple of the 64 KiB chunk
    const std::string path = make_temp_file(kSize);

    FileDescriptor fd = FileDescriptor::open(path.c_str(), O_RDONLY);
    EXPECT_EQ(file_size(fd.get()), kSize);

    std::vector<unsigned char> buf(kSize);
    EXPECT_EQ(read_whole_pread(fd.get(), buf.data(), kSize), kSize);
    for (std::size_t i = 0; i < kSize; i += 4096) {
        EXPECT_EQ(buf[i], expected_byte(i)) << "at offset " << i;
    }
    EXPECT_EQ(buf[kSize - 1], expected_byte(kSize - 1));

    ::unlink(path.c_str());
}

TEST(IoUtilsTest, MmapViewSeesSameContent) {
    constexpr std::size_t kSize = 3 * (64u << 10) + 17;  // spans a chunk boundary
    const std::string path = make_temp_file(kSize);

    FileDescriptor fd = FileDescriptor::open(path.c_str(), O_RDONLY);
    MmapView view(fd.get(), kSize, /*writable=*/false);
    ASSERT_EQ(view.size(), kSize);
    for (std::size_t i = 0; i < kSize; i += 8192) {
        EXPECT_EQ(view.data()[i], expected_byte(i));
    }
    EXPECT_EQ(view.data()[kSize - 1], expected_byte(kSize - 1));

    ::unlink(path.c_str());
}

TEST(IoUtilsTest, FileDescriptorMoveTransfersOwnership) {
    const std::string path = make_temp_file(16);
    FileDescriptor first = FileDescriptor::open(path.c_str(), O_RDONLY);
    const int raw = first.get();
    ASSERT_TRUE(first.valid());

    FileDescriptor second = std::move(first);
    EXPECT_FALSE(first.valid());
    EXPECT_EQ(second.get(), raw);

    second.reset();
    EXPECT_FALSE(second.valid());
    ::unlink(path.c_str());
}

}  // namespace hpc::io::test

#endif  // __linux__
