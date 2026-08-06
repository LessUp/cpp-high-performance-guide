/**
 * @file mmap_vs_read.cpp
 * @brief Three ways to read a whole file, compared on identical data:
 *        read() loop, pread() loop, and mmap + sequential touch.
 *
 * Key intuition:
 * - read()/pread() copy bytes from the page cache into userspace on every
 *   call; the syscall count scales with the buffer size you pick.
 * - mmap() sets up page-table mappings into the page cache with no upfront
 *   copy — bytes are faulted in as you touch them. For a full sequential
 *   scan both end up doing the same I/O, so the difference is syscall and
 *   copy overhead, not the disk.
 *
 * The file is read from the page cache (created moments earlier), so this
 * measures cache-resident read paths, not device bandwidth.
 *
 * Usage: mmap_vs_read [size_mib]   (default 64)
 */

#if !defined(__linux__)
#include <cstdio>
int main() {
    std::printf("mmap_vs_read requires Linux (POSIX mmap/pread).\n");
    return 0;
}
#else

#include <chrono>
#include <cstdio>
#include <cstring>
#include <hpc/io_utils.hpp>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

template <typename Fn>
double time_seconds(Fn&& fn) {
    const auto start = Clock::now();
    fn();
    return std::chrono::duration<double>(Clock::now() - start).count();
}

// FNV-1a keeps the compiler from deleting the read loops.
std::size_t checksum(const unsigned char* data, std::size_t n) {
    std::size_t hash = 1469598103934665603ull;
    for (std::size_t i = 0; i < n; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

constexpr std::size_t kBufferSize = 1u << 20;  // 1 MiB per syscall

std::size_t read_with_read_loop(int fd, std::vector<unsigned char>& buf, std::size_t size) {
    std::size_t done = 0;
    while (done < size) {
        const ssize_t r = ::read(fd, buf.data() + done, std::min(kBufferSize, size - done));
        if (r <= 0) {
            break;
        }
        done += static_cast<std::size_t>(r);
    }
    return done;
}

}  // namespace

int main(int argc, char** argv) {
    const std::size_t size_mib = argc > 1 ? static_cast<std::size_t>(std::atol(argv[1])) : 64;
    const std::size_t size = size_mib << 20;

    const std::string path = hpc::io::make_temp_file(size);
    std::printf("mmap vs read/pread over %zu MiB scratch file\n", size_mib);

    std::vector<unsigned char> buf(size);
    std::size_t reference_checksum = 0;

    {
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(path.c_str(), O_RDONLY);
        const double s = time_seconds([&] {
            read_with_read_loop(fd.get(), buf, size);
            reference_checksum = checksum(buf.data(), size);
        });
        std::printf("  read() loop  (1 MiB calls): %8.1f ms, %7.1f MiB/s, checksum %zx\n", s * 1e3,
                    static_cast<double>(size_mib) / s, reference_checksum);
    }

    std::size_t pread_checksum = 0;
    {
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(path.c_str(), O_RDONLY);
        const double s = time_seconds([&] {
            hpc::io::read_whole_pread(fd.get(), buf.data(), size);
            pread_checksum = checksum(buf.data(), size);
        });
        std::printf("  pread() loop (1 MiB calls): %8.1f ms, %7.1f MiB/s, checksum %zx\n", s * 1e3,
                    static_cast<double>(size_mib) / s, pread_checksum);
    }

    std::size_t mmap_checksum = 0;
    {
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(path.c_str(), O_RDONLY);
        const double s = time_seconds([&] {
            hpc::io::MmapView view(fd.get(), size, /*writable=*/false);
            mmap_checksum = checksum(view.data(), view.size());
        });
        std::printf("  mmap + touch (sequential):  %8.1f ms, %7.1f MiB/s, checksum %zx\n", s * 1e3,
                    static_cast<double>(size_mib) / s, mmap_checksum);
    }

    ::unlink(path.c_str());

    if (pread_checksum != reference_checksum || mmap_checksum != reference_checksum) {
        std::fprintf(stderr, "checksum mismatch between read paths\n");
        return 1;
    }
    std::printf("mmap_vs_read: OK (all three paths agree)\n");
    return 0;
}

#endif  // __linux__
