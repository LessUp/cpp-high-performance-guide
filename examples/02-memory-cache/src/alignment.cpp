/**
 * @file alignment.cpp
 * @brief Memory alignment for SIMD operations
 * 
 * This example demonstrates the importance of memory alignment for SIMD
 * operations. Aligned memory access is faster because:
 * 1. Aligned loads/stores can use efficient SIMD instructions
 * 2. Unaligned access may cross cache line boundaries
 * 3. Some older CPUs don't support unaligned SIMD at all
 * 
 * Key concepts:
 * - alignas() specifier
 * - posix_memalign / _aligned_malloc
 * - SIMD alignment requirements (16 for SSE, 32 for AVX, 64 for AVX-512)
 */

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#ifdef __AVX2__
#include <immintrin.h>
#endif

namespace hpc::memory {

constexpr size_t SIMD_ALIGNMENT = 32;  // AVX alignment

//------------------------------------------------------------------------------
// Aligned memory allocation
//------------------------------------------------------------------------------

/**
 * @brief Allocate aligned memory
 */
inline void* aligned_alloc(size_t size, size_t alignment) {
#if defined(_MSC_VER)
    return _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return nullptr;
    }
    return ptr;
#endif
}

/**
 * @brief Free aligned memory
 */
inline void aligned_free(void* ptr) {
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

/**
 * @brief RAII wrapper for aligned memory
 */
template<typename T>
class AlignedArray {
public:
    explicit AlignedArray(size_t count, size_t alignment = SIMD_ALIGNMENT)
        : size_(count)
        , data_(static_cast<T*>(aligned_alloc(count * sizeof(T), alignment)))
    {
        if (!data_) {
            throw std::bad_alloc();
        }
    }
    
    ~AlignedArray() {
        aligned_free(data_);
    }
    
    // Non-copyable
    AlignedArray(const AlignedArray&) = delete;
    AlignedArray& operator=(const AlignedArray&) = delete;
    
    // Movable
    AlignedArray(AlignedArray&& other) noexcept
        : size_(other.size_), data_(other.data_)
    {
        other.size_ = 0;
        other.data_ = nullptr;
    }
    
    T* data() { return data_; }
    const T* data() const { return data_; }
    size_t size() const { return size_; }
    
    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }
    
private:
    size_t size_;
    T* data_;
};

//------------------------------------------------------------------------------
// Vector addition implementations
//------------------------------------------------------------------------------

/**
 * @brief Scalar vector addition
 */
void add_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

#ifdef __AVX2__
/**
 * @brief AVX2 vector addition with aligned loads
 */
void add_avx_aligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    // Process 8 floats at a time with AVX
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_load_ps(a + i);   // Aligned load
        __m256 vb = _mm256_load_ps(b + i);   // Aligned load
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(c + i, vc);          // Aligned store
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

/**
 * @brief AVX2 vector addition with unaligned loads
 */
void add_avx_unaligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    
    // Process 8 floats at a time with AVX
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);  // Unaligned load
        __m256 vb = _mm256_loadu_ps(b + i);  // Unaligned load
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);         // Unaligned store
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}
#endif

//------------------------------------------------------------------------------
// Benchmark
//------------------------------------------------------------------------------

void run_benchmark() {
    constexpr size_t N = 10'000'000;
    constexpr int ITERATIONS = 100;
    
    // Aligned arrays
    AlignedArray<float> a_aligned(N);
    AlignedArray<float> b_aligned(N);
    AlignedArray<float> c_aligned(N);
    
    // Unaligned arrays (offset by 4 bytes to misalign)
    std::vector<float> buffer_a(N + 1);
    std::vector<float> buffer_b(N + 1);
    std::vector<float> buffer_c(N + 1);
    float* a_unaligned = buffer_a.data() + 1;  // Misaligned by 4 bytes
    float* b_unaligned = buffer_b.data() + 1;
    float* c_unaligned = buffer_c.data() + 1;
    
    // Initialize
    for (size_t i = 0; i < N; ++i) {
        float val_a = static_cast<float>(i % 1000) * 0.001f;
        float val_b = static_cast<float>((i + 500) % 1000) * 0.001f;
        a_aligned[i] = val_a;
        b_aligned[i] = val_b;
        a_unaligned[i] = val_a;
        b_unaligned[i] = val_b;
    }
    
    std::cout << "Array size: " << N << " floats\n";
    std::cout << "Iterations: " << ITERATIONS << "\n\n";
    
    std::cout << "Aligned array address:   " << a_aligned.data() 
              << " (aligned: " << (reinterpret_cast<uintptr_t>(a_aligned.data()) % 32 == 0) << ")\n";
    std::cout << "Unaligned array address: " << a_unaligned 
              << " (aligned: " << (reinterpret_cast<uintptr_t>(a_unaligned) % 32 == 0) << ")\n\n";
    
    // Scalar benchmark
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            add_scalar(a_aligned.data(), b_aligned.data(), c_aligned.data(), N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Scalar:        " << ms << " ms\n";
    }
    
#ifdef __AVX2__
    // AVX aligned benchmark
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            add_avx_aligned(a_aligned.data(), b_aligned.data(), c_aligned.data(), N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "AVX aligned:   " << ms << " ms\n";
    }
    
    // AVX unaligned benchmark
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            add_avx_unaligned(a_unaligned, b_unaligned, c_unaligned, N);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "AVX unaligned: " << ms << " ms\n";
    }
#else
    std::cout << "AVX2 not available - compile with -mavx2\n";
#endif
}

} // namespace hpc::memory

int main() {
    std::cout << "=== Memory Alignment for SIMD ===\n\n";
    hpc::memory::run_benchmark();
    
    std::cout << "\nNote: On modern CPUs, unaligned access penalty is small,\n";
    std::cout << "but aligned access is still preferred for best performance.\n";
    
    return 0;
}
