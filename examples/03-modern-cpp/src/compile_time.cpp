/**
 * @file compile_time.cpp
 * @brief Compile-time computation with constexpr and consteval
 *
 * This example demonstrates how to move computation from runtime to
 * compile time using C++20 constexpr and consteval features.
 *
 * Key concepts:
 * - constexpr: may be evaluated at compile time
 * - consteval: must be evaluated at compile time
 * - Compile-time lookup tables
 * - Template metaprogramming vs constexpr
 */

#include <chrono>
#include <cmath>
#include <hpc/compile_time.hpp>
#include <iostream>

// Example code lives in an anonymous namespace: the hpc::compile_time namespace is
// reserved for the canonical library (include/hpc/compile_time.hpp).
namespace {

// The user-defined literal operator cannot be qualified at the call site.
using hpc::compile_time::operator""_hash;

//------------------------------------------------------------------------------
// Demo
//------------------------------------------------------------------------------

void demonstrate_factorial() {
    std::cout << "=== Factorial ===\n";

    // Compile-time evaluation
    constexpr auto fact_10_compile = hpc::compile_time::factorial_constexpr(10);
    constexpr auto fact_10_eval = hpc::compile_time::factorial_consteval(10);

    std::cout << "10! (constexpr): " << fact_10_compile << "\n";
    std::cout << "10! (consteval): " << fact_10_eval << "\n";

    // Runtime evaluation
    int n = 10;
    auto fact_10_runtime = hpc::compile_time::factorial_runtime(n);
    std::cout << "10! (runtime):   " << fact_10_runtime << "\n";

    // Benchmark
    constexpr int ITERATIONS = 10'000'000;

    {
        auto start = std::chrono::steady_clock::now();
        volatile int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += hpc::compile_time::factorial_runtime(10);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Runtime factorial: " << ms << " ms\n";
    }

    {
        auto start = std::chrono::steady_clock::now();
        volatile int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            // This is evaluated at compile time, so the loop just adds a constant
            sum += hpc::compile_time::factorial_constexpr(10);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Constexpr factorial: " << ms << " ms\n";
    }
}

void demonstrate_lookup_table() {
    std::cout << "\n=== Sine Lookup Table ===\n";

    constexpr double PI = 3.14159265358979323846;

    // Compare accuracy
    std::cout << "Angle\t\tstd::sin\tfast_sin\tError\n";
    for (double angle : {0.0, PI / 6, PI / 4, PI / 3, PI / 2, PI}) {
        double std_sin = std::sin(angle);
        double fast = hpc::compile_time::fast_sin(angle);
        double error = std::abs(std_sin - fast);
        std::cout << angle << "\t\t" << std_sin << "\t" << fast << "\t" << error << "\n";
    }

    // Benchmark
    constexpr int ITERATIONS = 10'000'000;

    {
        auto start = std::chrono::steady_clock::now();
        volatile double sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += std::sin(static_cast<double>(i) * 0.001);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "std::sin: " << ms << " ms\n";
    }

    {
        auto start = std::chrono::steady_clock::now();
        volatile double sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += hpc::compile_time::fast_sin(static_cast<double>(i) * 0.001);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "fast_sin: " << ms << " ms\n";
    }
}

void demonstrate_string_hash() {
    std::cout << "\n=== Compile-time String Hashing ===\n";

    // These hashes are computed at compile time
    constexpr auto hash1 = "hello"_hash;
    constexpr auto hash2 = "world"_hash;
    constexpr auto hash3 = hpc::compile_time::fnv1a_hash("hello");

    std::cout << "Hash of 'hello': " << hash1 << "\n";
    std::cout << "Hash of 'world': " << hash2 << "\n";
    std::cout << "fnv1a('hello'):  " << hash3 << "\n";

    // Can be used in switch statements
    const char* test = "hello";
    switch (hpc::compile_time::fnv1a_hash(test)) {
        case "hello"_hash:
            std::cout << "Matched 'hello'!\n";
            break;
        case "world"_hash:
            std::cout << "Matched 'world'!\n";
            break;
        default:
            std::cout << "No match\n";
    }
}

void demonstrate_primes() {
    std::cout << "\n=== Compile-time Prime Generation ===\n";
    std::cout << "First 20 primes (computed at compile time):\n";
    for (size_t i = 0; i < 20; ++i) {
        std::cout << hpc::compile_time::FIRST_100_PRIMES[i] << " ";
    }
    std::cout << "\n";
    std::cout << "100th prime: " << hpc::compile_time::FIRST_100_PRIMES[99] << "\n";
}

}  // namespace

int main() {
    std::cout << "=== Compile-Time Computation Demo ===\n\n";

    demonstrate_factorial();
    demonstrate_lookup_table();
    demonstrate_string_hash();
    demonstrate_primes();

    return 0;
}
