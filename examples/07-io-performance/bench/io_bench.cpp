/**
 * @file io_bench.cpp
 * @brief Controlled read-path benchmark: read() loop vs pread() loop vs mmap.
 *
 * Each iteration re-reads a page-cache-resident scratch file, so numbers
 * reflect syscall/copy overhead rather than device bandwidth.
 */

#include <benchmark/benchmark.h>

#include <hpc/io_utils.hpp>

#if defined(__linux__)

#include <map>
#include <string>
#include <vector>

namespace {

constexpr std::size_t kBufferSize = 1u << 20;

std::size_t checksum(const unsigned char* data, std::size_t n) {
    std::size_t hash = 1469598103934665603ull;
    for (std::size_t i = 0; i < n; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

// One scratch file per benchmark size; created lazily and cached so setup
// cost stays out of the timed region.
const std::string& scratch_path(size_t size) {
    static std::map<size_t, std::string> cache;
    auto it = cache.find(size);
    if (it == cache.end()) {
        it = cache.emplace(size, hpc::io::make_temp_file(size)).first;
    }
    return it->second;
}

}  // namespace

static void BM_Read_Loop(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    const std::string& path = scratch_path(size);
    std::vector<unsigned char> buf(size);

    for (auto _ : state) {
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(path.c_str(), O_RDONLY);
        size_t done = 0;
        while (done < size) {
            const ssize_t r =
                ::read(fd.get(), buf.data() + done, std::min(kBufferSize, size - done));
            if (r <= 0) {
                break;
            }
            done += static_cast<size_t>(r);
        }
        benchmark::DoNotOptimize(checksum(buf.data(), done));
    }

    state.SetBytesProcessed(state.iterations() * size);
}

static void BM_Read_Pread(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    const std::string& path = scratch_path(size);
    std::vector<unsigned char> buf(size);

    for (auto _ : state) {
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(path.c_str(), O_RDONLY);
        const size_t read = hpc::io::read_whole_pread(fd.get(), buf.data(), size);
        benchmark::DoNotOptimize(checksum(buf.data(), read));
    }

    state.SetBytesProcessed(state.iterations() * size);
}

static void BM_Read_Mmap(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    const std::string& path = scratch_path(size);

    for (auto _ : state) {
        hpc::io::FileDescriptor fd = hpc::io::FileDescriptor::open(path.c_str(), O_RDONLY);
        hpc::io::MmapView view(fd.get(), size, /*writable=*/false);
        benchmark::DoNotOptimize(checksum(view.data(), view.size()));
    }

    state.SetBytesProcessed(state.iterations() * size);
}

BENCHMARK(BM_Read_Loop)->RangeMultiplier(4)->Range(1 << 20, 1 << 27);
BENCHMARK(BM_Read_Pread)->RangeMultiplier(4)->Range(1 << 20, 1 << 27);
BENCHMARK(BM_Read_Mmap)->RangeMultiplier(4)->Range(1 << 20, 1 << 27);

#else  // non-Linux placeholder so the file still compiles everywhere

static void BM_IoUnavailable(benchmark::State& state) {
    for (auto _ : state) {
        state.SkipWithError("io_bench requires Linux");
        break;
    }
}
BENCHMARK(BM_IoUnavailable);

#endif  // __linux__
