/**
 * @file buffered_write.cpp
 * @brief Write-path syscall overhead: 1-byte writes vs buffered chunks vs a
 *        single bulk write of the same data.
 *
 * Each write() is a context switch into the kernel. When the buffer is tiny,
 * you pay that fixed cost per byte and throughput collapses; chunking the
 * same bytes into larger writes removes almost all of it. This is why
 * fwrite()/std::ofstream buffer internally, and why O_DIRECT or unbuffered
 * logging hurts.
 *
 * The file stays in the page cache — this measures syscall overhead, not
 * device write speed.
 */

#if !defined(__linux__)
#include <cstdio>
int main() {
    std::printf("buffered_write requires Linux (POSIX write/unlink).\n");
    return 0;
}
#else

#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <hpc/io_utils.hpp>
#include <vector>

namespace {

constexpr std::size_t kTotalBytes = 1u << 20;  // 1 MiB of identical payload

using Clock = std::chrono::steady_clock;

template <typename Fn>
double time_seconds(Fn&& fn) {
    const auto start = Clock::now();
    fn();
    return std::chrono::duration<double>(Clock::now() - start).count();
}

std::string fresh_path(const char* suffix) {
    return std::string("/tmp/hpc_io_write_") + suffix;
}

}  // namespace

int main() {
    std::vector<unsigned char> payload(kTotalBytes);
    for (std::size_t i = 0; i < kTotalBytes; ++i) {
        payload[i] = static_cast<unsigned char>(i & 0xFF);
    }

    std::printf("writing %zu KiB three ways\n", kTotalBytes >> 10);

    // 1) One syscall per byte: worst case. Kept at 1 MiB so it stays quick.
    {
        const std::string path = fresh_path("byte");
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(
            path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, /*mode=*/0644);
        const double s = time_seconds([&] {
            for (std::size_t i = 0; i < kTotalBytes; ++i) {
                ::write(fd.get(), &payload[i], 1);
            }
        });
        std::printf("  1-byte writes   (%8zu syscalls): %9.1f ms, %7.1f MiB/s\n", kTotalBytes,
                    s * 1e3, (kTotalBytes >> 20) / s);
        ::unlink(path.c_str());
    }

    // 2) 4 KiB buffered chunks: what fwrite/ofstream do under the hood.
    {
        const std::string path = fresh_path("chunk");
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(
            path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, /*mode=*/0644);
        constexpr std::size_t kChunk = 4096;
        const double s = time_seconds([&] {
            for (std::size_t i = 0; i < kTotalBytes; i += kChunk) {
                ::write(fd.get(), payload.data() + i, kChunk);
            }
        });
        std::printf("  4 KiB chunks    (%8zu syscalls): %9.1f ms, %7.1f MiB/s\n",
                    kTotalBytes / kChunk, s * 1e3, (kTotalBytes >> 20) / s);
        ::unlink(path.c_str());
    }

    // 3) One bulk write: a single syscall for everything.
    {
        const std::string path = fresh_path("bulk");
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(
            path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, /*mode=*/0644);
        const double s = time_seconds([&] { ::write(fd.get(), payload.data(), kTotalBytes); });
        std::printf("  single bulk     (%8d syscall):  %9.1f ms, %7.1f MiB/s\n", 1, s * 1e3,
                    (kTotalBytes >> 20) / s);
        ::unlink(path.c_str());
    }

    std::printf("buffered_write: OK\n");
    return 0;
}

#endif  // __linux__
