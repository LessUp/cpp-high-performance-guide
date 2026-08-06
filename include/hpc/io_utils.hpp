#pragma once

/**
 * @file io_utils.hpp
 * @brief POSIX file I/O helpers for the 07-io-performance examples: an RAII
 *        file descriptor, an RAII mmap view, and small read helpers.
 *
 * Linux/POSIX only. The header is empty on other platforms so including it
 * stays harmless; the consuming examples gate on __linux__ as well.
 */

#if defined(__linux__)

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace hpc::io {

// RAII wrapper around a POSIX file descriptor.
class FileDescriptor {
public:
    FileDescriptor() = default;

    static FileDescriptor open(const char* path, int flags, int mode = 0) {
        // mode only matters when flags contains O_CREAT; passing it always
        // keeps callers from the classic missing-mode UB.
        const int fd = ::open(path, flags, static_cast<mode_t>(mode));
        if (fd < 0) {
            throw std::runtime_error(std::string("open failed: ") + path);
        }
        return FileDescriptor(fd);
    }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

    FileDescriptor& operator=(FileDescriptor&& other) noexcept {
        if (this != &other) {
            reset();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    ~FileDescriptor() { reset(); }

    void reset() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    int get() const { return fd_; }
    bool valid() const { return fd_ >= 0; }

private:
    explicit FileDescriptor(int fd) : fd_(fd) {}
    int fd_ = -1;
};

// RAII view over a file mapped into memory. MAP_SHARED so writes (when
// requested) land in the page cache and reach the file.
class MmapView {
public:
    MmapView(int fd, std::size_t size, bool writable) : size_(size) {
        const int prot = writable ? (PROT_READ | PROT_WRITE) : PROT_READ;
        void* mapped = ::mmap(nullptr, size, prot, MAP_SHARED, fd, 0);
        if (mapped == MAP_FAILED) {
            throw std::runtime_error("mmap failed");
        }
        data_ = static_cast<unsigned char*>(mapped);
    }

    MmapView(const MmapView&) = delete;
    MmapView& operator=(const MmapView&) = delete;

    MmapView(MmapView&& other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    ~MmapView() {
        if (data_ != nullptr) {
            ::munmap(data_, size_);
        }
    }

    const unsigned char* data() const { return data_; }
    unsigned char* data() { return data_; }
    std::size_t size() const { return size_; }

private:
    unsigned char* data_ = nullptr;
    std::size_t size_ = 0;
};

inline std::size_t file_size(int fd) {
    struct stat st {};
    if (::fstat(fd, &st) != 0) {
        throw std::runtime_error("fstat failed");
    }
    return static_cast<std::size_t>(st.st_size);
}

// Reads the whole file into an already-sized buffer using pread (does not
// move the file offset). Returns the number of bytes actually read.
inline std::size_t read_whole_pread(int fd, unsigned char* buf, std::size_t size) {
    std::size_t done = 0;
    while (done < size) {
        const ssize_t r = ::pread(fd, buf + done, size - done, static_cast<off_t>(done));
        if (r <= 0) {
            break;
        }
        done += static_cast<std::size_t>(r);
    }
    return done;
}

// Creates a scratch file of `size` bytes filled with a deterministic
// position-dependent pattern, and returns its path. The caller owns the
// file and must unlink it.
inline std::string make_temp_file(std::size_t size) {
    char tmpl[] = "/tmp/hpc_io_XXXXXX";
    const int fd = ::mkstemp(tmpl);
    if (fd < 0) {
        throw std::runtime_error("mkstemp failed");
    }

    constexpr std::size_t kChunk = 64u << 10;
    std::vector<unsigned char> chunk(kChunk);
    std::size_t written = 0;
    while (written < size) {
        const std::size_t n = std::min(kChunk, size - written);
        for (std::size_t i = 0; i < n; ++i) {
            chunk[i] = static_cast<unsigned char>((written + i) & 0xFF);
        }
        if (::write(fd, chunk.data(), n) != static_cast<ssize_t>(n)) {
            ::close(fd);
            ::unlink(tmpl);
            throw std::runtime_error("short write while creating temp file");
        }
        written += n;
    }
    ::close(fd);
    return std::string(tmpl);
}

}  // namespace hpc::io

#endif  // __linux__
