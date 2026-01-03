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

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>

namespace hpc::compile_time {

//------------------------------------------------------------------------------
// Compile-time factorial
//------------------------------------------------------------------------------

/**
 * @brief Runtime factorial (for comparison)
 */
int64_t factorial_runtime(int n) {
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
template<size_t N>
constexpr std::array<double, N> generate_sin_table() {
    std::array<double, N> table{};
    constexpr double PI = 3.14159265358979323846;
    for (size_t i = 0; i < N; ++i) {
        double angle = (2.0 * PI * static_cast<double>(i)) / static_cast<double>(N);
        // Taylor series approximation for sin (constexpr-friendly)
        double x = angle;
        // Normalize to [-PI, PI]
        while (x > PI) x -= 2.0 * PI;
        while (x < -PI) x += 2.0 * PI;
        
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

// Compile-time generated lookup table
constexpr auto SIN_TABLE = generate_sin_table<1024>();

/**
 * @brief Fast sine using compile-time lookup table
 */
double fast_sin(double angle) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    constexpr size_t TABLE_SIZE = SIN_TABLE.size();
    
    // Normalize angle to [0, 2*PI)
    while (angle < 0) angle += TWO_PI;
    while (angle >= TWO_PI) angle -= TWO_PI;
    
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

constexpr bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

/**
 * @brief Generate array of first N primes at compile time
 */
template<size_t N>
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

// First 100 primes computed at compile time
constexpr auto FIRST_100_PRIMES = generate_primes<100>();

//------------------------------------------------------------------------------
// Demo
//------------------------------------------------------------------------------

void demonstrate_factorial() {
    std::cout << "=== Factorial ===\n";
    
    // Compile-time evaluation
    constexpr auto fact_10_compile = factorial_constexpr(10);
    constexpr auto fact_10_eval = factorial_consteval(10);
    
    std::cout << "10! (constexpr): " << fact_10_compile << "\n";
    std::cout << "10! (consteval): " << fact_10_eval << "\n";
    
    // Runtime evaluation
    int n = 10;
    auto fact_10_runtime = factorial_runtime(n);
    std::cout << "10! (runtime):   " << fact_10_runtime << "\n";
    
    // Benchmark
    constexpr int ITERATIONS = 10'000'000;
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        volatile int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += factorial_runtime(10);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Runtime factorial: " << ms << " ms\n";
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        volatile int64_t sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            // This is evaluated at compile time, so the loop just adds a constant
            sum += factorial_constexpr(10);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Constexpr factorial: " << ms << " ms\n";
    }
}

void demonstrate_lookup_table() {
    std::cout << "\n=== Sine Lookup Table ===\n";
    
    constexpr double PI = 3.14159265358979323846;
    
    // Compare accuracy
    std::cout << "Angle\t\tstd::sin\tfast_sin\tError\n";
    for (double angle : {0.0, PI/6, PI/4, PI/3, PI/2, PI}) {
        double std_sin = std::sin(angle);
        double fast = fast_sin(angle);
        double error = std::abs(std_sin - fast);
        std::cout << angle << "\t\t" << std_sin << "\t" << fast << "\t" << error << "\n";
    }
    
    // Benchmark
    constexpr int ITERATIONS = 10'000'000;
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        volatile double sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += std::sin(static_cast<double>(i) * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "std::sin: " << ms << " ms\n";
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        volatile double sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += fast_sin(static_cast<double>(i) * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "fast_sin: " << ms << " ms\n";
    }
}

void demonstrate_string_hash() {
    std::cout << "\n=== Compile-time String Hashing ===\n";
    
    // These hashes are computed at compile time
    constexpr auto hash1 = "hello"_hash;
    constexpr auto hash2 = "world"_hash;
    constexpr auto hash3 = fnv1a_hash("hello");
    
    std::cout << "Hash of 'hello': " << hash1 << "\n";
    std::cout << "Hash of 'world': " << hash2 << "\n";
    std::cout << "fnv1a('hello'):  " << hash3 << "\n";
    
    // Can be used in switch statements
    const char* test = "hello";
    switch (fnv1a_hash(test)) {
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
        std::cout << FIRST_100_PRIMES[i] << " ";
    }
    std::cout << "\n";
    std::cout << "100th prime: " << FIRST_100_PRIMES[99] << "\n";
}

} // namespace hpc::compile_time

int main() {
    std::cout << "=== Compile-Time Computation Demo ===\n\n";
    
    hpc::compile_time::demonstrate_factorial();
    hpc::compile_time::demonstrate_lookup_table();
    hpc::compile_time::demonstrate_string_hash();
    hpc::compile_time::demonstrate_primes();
    
    return 0;
}
