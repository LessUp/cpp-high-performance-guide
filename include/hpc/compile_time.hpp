/**
 * @file compile_time.hpp
 * @brief Compile-time computation utilities
 *
 * This header provides utilities for compile-time computation using
 * C++20 constexpr and consteval features.
 *
 * Key concepts:
 * - constexpr: may be evaluated at compile time
 * - consteval: must be evaluated at compile time
 * - Compile-time lookup tables
 */

#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace hpc::compile_time {

//------------------------------------------------------------------------------
// Compile-time factorial
//------------------------------------------------------------------------------

/**
 * @brief Runtime factorial (for comparison)
 * @pre n <= 20 (21! overflows int64_t, which is UB at runtime)
 */
inline int64_t factorial_runtime(int n) {
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

/**
 * @brief Compile-time factorial using constexpr
 *
 * Can be evaluated at compile time if argument is known at compile time.
 * @pre n <= 20 (21! overflows int64_t; a constexpr evaluation would fail)
 */
constexpr int64_t factorial_constexpr(int n) {
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

/**
 * @brief Compile-time factorial using consteval (C++20)
 *
 * MUST be evaluated at compile time. Compiler error if called with
 * runtime value.
 * @pre n <= 20 (21! overflows int64_t; a constexpr evaluation would fail)
 */
consteval int64_t factorial_consteval(int n) {
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

//------------------------------------------------------------------------------
// Compile-time lookup tables
//------------------------------------------------------------------------------

/**
 * @brief Generate sine lookup table at compile time
 */
template <size_t N>
constexpr std::array<double, N> generate_sin_table() {
    std::array<double, N> table{};
    constexpr double PI = 3.14159265358979323846;
    for (size_t i = 0; i < N; ++i) {
        double angle = (2.0 * PI * static_cast<double>(i)) / static_cast<double>(N);
        // Taylor series approximation for sin (constexpr-friendly)
        double x = angle;
        // Normalize to [-PI, PI]
        while (x > PI)
            x -= 2.0 * PI;
        while (x < -PI)
            x += 2.0 * PI;

        // Taylor series: sin(x) = x - x^3/3! + x^5/5! - x^7/7! + ...
        double x2 = x * x;
        double term = x;
        double sum = x;
        for (int n = 1; n < 10; ++n) {
            term *= -x2 / static_cast<double>((2 * n) * (2 * n + 1));
            sum += term;
        }
        table[i] = sum;
    }
    return table;
}

// Compile-time generated lookup table (inline: one copy program-wide, not per TU)
inline constexpr auto SIN_TABLE = generate_sin_table<1024>();

/**
 * @brief Fast sine using compile-time lookup table
 *
 * @note Not constexpr: std::fmod-based range reduction is a runtime operation.
 */
inline double fast_sin(double angle) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    constexpr size_t TABLE_SIZE = SIN_TABLE.size();

    // Match std::sin semantics for non-finite input. Without this guard the
    // index computation below would cast NaN to size_t, which is UB.
    if (!std::isfinite(angle)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    // Normalize angle to [0, 2*PI) with std::fmod. The previous reduction
    // (static_cast<int64_t>(angle / TWO_PI)) is UB once |angle| exceeds the
    // int64_t range, e.g. fast_sin(1e308).
    angle = std::fmod(angle, TWO_PI);
    if (angle < 0)
        angle += TWO_PI;

    // Convert to table index
    size_t index = static_cast<size_t>((angle / TWO_PI) * TABLE_SIZE) % TABLE_SIZE;
    return SIN_TABLE[index];
}

//------------------------------------------------------------------------------
// Compile-time string hashing
//------------------------------------------------------------------------------

/**
 * @brief FNV-1a hash at compile time
 */
constexpr uint64_t fnv1a_hash(const char* str) {
    constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    uint64_t hash = FNV_OFFSET;
    while (*str) {
        hash ^= static_cast<uint64_t>(*str++);
        hash *= FNV_PRIME;
    }
    return hash;
}

/**
 * @brief String literal hash operator
 */
consteval uint64_t operator""_hash(const char* str, size_t) {
    return fnv1a_hash(str);
}

//------------------------------------------------------------------------------
// Compile-time prime checking
//------------------------------------------------------------------------------

/**
 * @brief Check if a number is prime at compile time
 */
constexpr bool is_prime(int n) {
    if (n < 2)
        return false;
    if (n == 2)
        return true;
    if (n % 2 == 0)
        return false;
    // i <= n / i instead of i * i <= n: the multiplication overflows signed
    // int for n near INT_MAX (UB at runtime, hard error in constexpr), and
    // made is_prime(2147483647) wrongly report "not prime".
    for (int i = 3; i <= n / i; i += 2) {
        if (n % i == 0)
            return false;
    }
    return true;
}

/**
 * @brief Generate array of first N primes at compile time
 */
template <size_t N>
constexpr std::array<int, N> generate_primes() {
    std::array<int, N> primes{};
    int count = 0;
    int candidate = 2;
    while (count < static_cast<int>(N)) {
        if (is_prime(candidate)) {
            primes[static_cast<size_t>(count++)] = candidate;
        }
        ++candidate;
    }
    return primes;
}

// First 100 primes computed at compile time (inline: one copy program-wide)
inline constexpr auto FIRST_100_PRIMES = generate_primes<100>();

}  // namespace hpc::compile_time
