#pragma once

/**
 * @file gemm.hpp
 * @brief Square matrix multiply C = A*B (row-major, float) in four
 *        optimization stages, used by the 09-matrix-multiply case study:
 *
 *   naive    — ijk loop order; B is walked column-wise, cache-hostile
 *   tiled    — cache blocking over (i, k, j), cache-friendly inside blocks
 *   simd     — tiled + vectorized micro-kernel along j (FMA)
 *   parallel — simd + OpenMP over rows (falls back to simd without OpenMP)
 *
 * All variants compute the same result; callers can verify each stage
 * against naive. No alignment or size preconditions: tails are scalar.
 */

#include <cstddef>

#if defined(__AVX2__) && defined(__FMA__)
#include <immintrin.h>
#define HPC_GEMM_HAS_AVX2 1
#elif defined(__aarch64__) && (defined(__GNUC__) || defined(__clang__))
#include <arm_neon.h>
#define HPC_GEMM_HAS_NEON 1
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

namespace hpc::gemm {

// Stage 1: textbook triple loop in i,j,k order. The inner k loop walks B
// with stride n (column-wise), so every access misses the cache line that
// the previous iteration just loaded. This is the "why is my GEMM slow"
// baseline.
inline void naive(const float* A, const float* B, float* C, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < n; ++k) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

// Stage 2: cache blocking. Process the matrices in block_size x block_size
// tiles; within a tile the loop order is i,k,j so both B rows and C rows are
// walked contiguously and the tile stays resident in cache.
inline void tiled(const float* A, const float* B, float* C, int n, int block_size = 64) {
    for (int i0 = 0; i0 < n; i0 += block_size) {
        for (int k0 = 0; k0 < n; k0 += block_size) {
            for (int j0 = 0; j0 < n; j0 += block_size) {
                const int i_end = i0 + block_size < n ? i0 + block_size : n;
                const int k_end = k0 + block_size < n ? k0 + block_size : n;
                const int j_end = j0 + block_size < n ? j0 + block_size : n;
                for (int i = i0; i < i_end; ++i) {
                    for (int k = k0; k < k_end; ++k) {
                        const float a_ik = A[i * n + k];
                        for (int j = j0; j < j_end; ++j) {
                            C[i * n + j] += a_ik * B[k * n + j];
                        }
                    }
                }
            }
        }
    }
}

namespace detail {

// One row of C, accumulated over k in [k0, k_end): C[i][j] += A[i][k]*B[k][j].
// Vectorized along j with FMA; scalar tail. This is the micro-kernel shared
// by the simd and parallel stages.
inline void accumulate_row_krange(const float* A, const float* B, float* C, int n, int i,
                                  int k0, int k_end) {
    const float* a_row = A + static_cast<std::size_t>(i) * n;
    float* c_row = C + static_cast<std::size_t>(i) * n;

    for (int k = k0; k < k_end; ++k) {
        const float a_ik = a_row[k];
        const float* b_row = B + static_cast<std::size_t>(k) * n;
        int j = 0;

#if defined(HPC_GEMM_HAS_AVX2)
        const __m256 va = _mm256_broadcast_ss(&a_ik);
        for (; j + 8 <= n; j += 8) {
            const __m256 vb = _mm256_loadu_ps(b_row + j);
            const __m256 vc = _mm256_loadu_ps(c_row + j);
            _mm256_storeu_ps(c_row + j, _mm256_fmadd_ps(va, vb, vc));
        }
#elif defined(HPC_GEMM_HAS_NEON)
        const float32x4_t va = vdupq_n_f32(a_ik);
        for (; j + 4 <= n; j += 4) {
            const float32x4_t vb = vld1q_f32(b_row + j);
            const float32x4_t vc = vld1q_f32(c_row + j);
            vst1q_f32(c_row + j, vfmaq_f32(vc, va, vb));
        }
#endif
        for (; j < n; ++j) {
            c_row[j] += a_ik * b_row[j];
        }
    }
}

}  // namespace detail

// Stage 3: tiled + SIMD. k is blocked for cache behavior, the j direction is
// vectorized with FMA (8 floats/instruction on AVX2, 4 on NEON).
inline void simd(const float* A, const float* B, float* C, int n, int block_size = 64) {
    for (int k0 = 0; k0 < n; k0 += block_size) {
        const int k_end = k0 + block_size < n ? k0 + block_size : n;
        for (int i = 0; i < n; ++i) {
            detail::accumulate_row_krange(A, B, C, n, i, k0, k_end);
        }
    }
}

// Stage 4: SIMD + OpenMP over rows. Without OpenMP this is just simd().
inline void parallel(const float* A, const float* B, float* C, int n, int block_size = 64) {
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        for (int k0 = 0; k0 < n; k0 += block_size) {
            const int k_end = k0 + block_size < n ? k0 + block_size : n;
            detail::accumulate_row_krange(A, B, C, n, i, k0, k_end);
        }
    }
#else
    simd(A, B, C, n, block_size);
#endif
}

// Zero-initialize C before any stage (all stages accumulate into C).
inline void zero(float* C, int n) {
    for (std::size_t idx = 0; idx < static_cast<std::size_t>(n) * n; ++idx) {
        C[idx] = 0.0f;
    }
}

}  // namespace hpc::gemm
