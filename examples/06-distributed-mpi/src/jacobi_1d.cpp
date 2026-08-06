/**
 * @file jacobi_1d.cpp
 * @brief 1D Jacobi relaxation with domain decomposition and halo exchange —
 *        the canonical "first real MPI program".
 *
 * Solves the 1D Laplace relaxation  u[i] <- (u[i-1] + u[i+1]) / 2  on the
 * interior points with fixed boundary values (left = 1, right = 0), marching
 * the stencil for a fixed number of iterations.
 *
 * The global row is split into contiguous blocks, one per rank. Each rank
 * keeps one ghost cell on each side that mirrors the neighbour's boundary
 * point; ghosts are refreshed with a halo exchange before every update.
 *
 * Self-verification: after the parallel run, rank 0 gathers the full result
 * and compares it against a sequential full-domain run of the same stencil.
 * Both perform identical arithmetic per point, so the results must agree.
 *
 * Run with:  mpirun -np 4 ./jacobi_1d
 */

#include <mpi.h>

#include <cmath>
#include <cstdio>
#include <vector>

namespace {

constexpr int kGlobalPoints = 4096;  // interior points (excluding boundaries)
constexpr int kIterations = 500;
constexpr double kLeftBoundary = 1.0;
constexpr double kRightBoundary = 0.0;

// Contiguous block decomposition: the first `remainder` ranks take one extra
// point. Returns the number of owned points for `rank`.
int block_size(int rank, int size) {
    const int base = kGlobalPoints / size;
    const int remainder = kGlobalPoints % size;
    return base + (rank < remainder ? 1 : 0);
}

// Global index of the first point owned by `rank`.
int block_offset(int rank, int size) {
    const int base = kGlobalPoints / size;
    const int remainder = kGlobalPoints % size;
    if (rank < remainder) {
        return rank * (base + 1);
    }
    return remainder * (base + 1) + (rank - remainder) * base;
}

// One halo exchange: swap boundary points with the direct neighbours.
// Both ends of every edge use the same tag and post a matching Sendrecv, so
// the pairwise exchange cannot deadlock regardless of issue order.
void exchange_halos(std::vector<double>& u, int count, int rank, int size) {
    const int left = rank - 1;
    const int right = rank + 1;

    if (left >= 0) {
        // Send leftmost owned cell leftwards, receive the neighbour's
        // rightmost cell into the left ghost slot.
        MPI_Sendrecv(&u[1], 1, MPI_DOUBLE, left, /*tag=*/0, &u[0], 1, MPI_DOUBLE, left,
                     /*tag=*/0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if (right < size) {
        MPI_Sendrecv(&u[count], 1, MPI_DOUBLE, right, /*tag=*/0, &u[count + 1], 1, MPI_DOUBLE,
                     right, /*tag=*/0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

double jacobi_update_step(const std::vector<double>& u, std::vector<double>& u_new, int count) {
    // u layout: [ghost_left | owned 1..count | ghost_right]
    double local_max_delta = 0.0;
    for (int i = 1; i <= count; ++i) {
        const double next =
            0.5 * (u[static_cast<std::size_t>(i - 1)] + u[static_cast<std::size_t>(i + 1)]);
        local_max_delta =
            std::max(local_max_delta, std::fabs(next - u[static_cast<std::size_t>(i)]));
        u_new[static_cast<std::size_t>(i)] = next;
    }
    return local_max_delta;
}

// Sequential reference over the whole domain (rank 0 only).
std::vector<double> sequential_reference() {
    std::vector<double> u(static_cast<std::size_t>(kGlobalPoints) + 2, 0.0);
    std::vector<double> u_new(u.size(), 0.0);
    u[0] = kLeftBoundary;
    u[u.size() - 1] = kRightBoundary;
    u_new[0] = kLeftBoundary;
    u_new[u_new.size() - 1] = kRightBoundary;

    for (int it = 0; it < kIterations; ++it) {
        for (int i = 1; i <= kGlobalPoints; ++i) {
            u_new[static_cast<std::size_t>(i)] =
                0.5 * (u[static_cast<std::size_t>(i - 1)] + u[static_cast<std::size_t>(i + 1)]);
        }
        std::swap(u, u_new);
    }
    return u;
}

}  // namespace

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int count = block_size(rank, size);

    // Local buffers carry one ghost cell on each side. Interior starts from
    // the left boundary value so the first iteration has real data to relax.
    std::vector<double> u(static_cast<std::size_t>(count) + 2, 0.0);
    std::vector<double> u_new(u.size(), 0.0);
    if (rank == 0) {
        u[0] = kLeftBoundary;
        u_new[0] = kLeftBoundary;
    }
    if (rank == size - 1) {
        u[static_cast<std::size_t>(count) + 1] = kRightBoundary;
        u_new[static_cast<std::size_t>(count) + 1] = kRightBoundary;
    }

    const double start = MPI_Wtime();
    double max_delta = 0.0;
    for (int it = 0; it < kIterations; ++it) {
        if (size > 1) {
            exchange_halos(u, count, rank, size);
        } else {
            // np == 1: ghosts are the physical boundaries.
            u[0] = kLeftBoundary;
            u[static_cast<std::size_t>(count) + 1] = kRightBoundary;
        }
        const double local_delta = jacobi_update_step(u, u_new, count);
        max_delta = std::max(max_delta, local_delta);
        std::swap(u, u_new);
    }
    const double elapsed = MPI_Wtime() - start;

    // Gather counts and displacements for Gatherv.
    std::vector<int> counts(static_cast<std::size_t>(size));
    std::vector<int> displs(static_cast<std::size_t>(size));
    for (int r = 0; r < size; ++r) {
        counts[static_cast<std::size_t>(r)] = block_size(r, size);
        displs[static_cast<std::size_t>(r)] = block_offset(r, size);
    }

    std::vector<double> global(static_cast<std::size_t>(kGlobalPoints), 0.0);
    MPI_Gatherv(&u[1], count, MPI_DOUBLE, global.data(), counts.data(), displs.data(), MPI_DOUBLE,
                /*root=*/0, MPI_COMM_WORLD);

    double global_delta = 0.0;
    MPI_Reduce(&max_delta, &global_delta, 1, MPI_DOUBLE, MPI_MAX, /*root=*/0, MPI_COMM_WORLD);

    int verdict = 0;
    if (rank == 0) {
        const std::vector<double> reference = sequential_reference();
        double max_diff = 0.0;
        for (int i = 0; i < kGlobalPoints; ++i) {
            max_diff = std::max(max_diff, std::fabs(global[static_cast<std::size_t>(i)] -
                                                    reference[static_cast<std::size_t>(i + 1)]));
        }
        verdict = max_diff <= 1e-9 ? 0 : 1;
        std::printf("jacobi_1d: %d points, %d iterations, %d ranks\n", kGlobalPoints, kIterations,
                    size);
        std::printf("  wall time %.3f ms, last max update %.3e\n", elapsed * 1e3, global_delta);
        std::printf("  vs sequential reference: max diff %.3e -> %s\n", max_diff,
                    verdict == 0 ? "OK" : "FAILED");
    }

    MPI_Bcast(&verdict, 1, MPI_INT, /*root=*/0, MPI_COMM_WORLD);
    MPI_Finalize();
    return verdict;
}
