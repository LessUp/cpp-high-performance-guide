/**
 * @file core.hpp
 * @brief Core platform detection utilities for HPC optimization
 *
 * Provides platform-independent constants and detection functions
 * for cache line sizes, page sizes, and other system properties.
 *
 * This header is designed to be a minimal dependency that can be
 * included by other modules without introducing circular dependencies.
 */

#pragma once

#include <cstddef>
#include <thread>

// POSIX headers for sysconf
#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

namespace hpc::core {

//------------------------------------------------------------------------------
// Cache Line Size
//------------------------------------------------------------------------------

/**
 * @brief Get the cache line size at runtime
 *
 * Uses std::hardware_destructive_interference_size when available (C++17),
 * otherwise returns a reasonable default for the platform.
 *
 * @return Cache line size in bytes
 */
inline std::size_t cache_line_size() {
#if defined(__cpp_lib_hardware_interference_size)
    return std::hardware_destructive_interference_size;
#else
    // Most x86/ARM systems use 64-byte cache lines
    // Some ARM systems (e.g., Apple M1) use 128-byte
    // This is a conservative default
    return 64;
#endif
}

/**
 * @brief Compile-time cache line size constant
 *
 * Use this when the value must be a compile-time constant (e.g., alignas).
 * For runtime detection, prefer cache_line_size() function.
 */
#if defined(__cpp_lib_hardware_interference_size)
constexpr std::size_t CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
#else
constexpr std::size_t CACHE_LINE_SIZE = 64;
#endif

//------------------------------------------------------------------------------
// Page Size
//------------------------------------------------------------------------------

/**
 * @brief Get the system page size at runtime
 *
 * Returns the system page size, with 4096 as a fallback.
 *
 * @return Page size in bytes
 */
inline std::size_t page_size() {
    static const std::size_t ps = []() {
#if defined(_WIN32)
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return static_cast<std::size_t>(sysInfo.dwPageSize);
#elif defined(__unix__) || defined(__APPLE__)
        long ps = sysconf(_SC_PAGESIZE);
        return ps > 0 ? static_cast<std::size_t>(ps) : 4096;
#else
        return 4096;  // Fallback
#endif
    }();
    return ps;
}

/**
 * @brief Compile-time page size constant for most systems
 *
 * Use this when the value must be a compile-time constant.
 * For runtime detection, prefer page_size() function.
 */
constexpr std::size_t PAGE_SIZE = 4096;

//------------------------------------------------------------------------------
// Hardware Concurrency
//------------------------------------------------------------------------------

/**
 * @brief Get the number of hardware threads
 *
 * @return Number of concurrent threads supported by the hardware
 */
inline unsigned int hardware_concurrency() {
    unsigned int n = std::thread::hardware_concurrency();
    return n > 0 ? n : 1;
}

}  // namespace hpc::core

//------------------------------------------------------------------------------
// Backward Compatibility
//------------------------------------------------------------------------------

// Backward compatibility: bring into hpc::memory namespace
// Note: New code should use hpc::core::CACHE_LINE_SIZE and hpc::core::PAGE_SIZE directly
namespace hpc::memory {
using hpc::core::CACHE_LINE_SIZE;
using hpc::core::PAGE_SIZE;
}  // namespace hpc::memory

// Backward compatibility: bring into hpc::concurrency namespace
namespace hpc::concurrency {
using hpc::core::CACHE_LINE_SIZE;
}  // namespace hpc::concurrency
