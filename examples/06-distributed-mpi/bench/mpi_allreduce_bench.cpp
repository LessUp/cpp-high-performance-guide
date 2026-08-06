/**
 * @file mpi_allreduce_bench.cpp
 * @brief Allreduce latency vs message size across all ranks.
 *
 * Allreduce is the collective that dominates large-scale training and
 * solver runtimes, so it deserves its own measurement. Like the ping-pong
 * benchmark this uses manual MPI_Wtime() loops — see mpi_pingpong_bench.cpp
 * for why Google Benchmark is not used for collective communication.
 *
 * Run with:  mpirun -np 4 ./mpi_allreduce_bench
 */

#include <mpi.h>

#include <cstdio>
#include <vector>

namespace {

constexpr int kWarmup = 10;
constexpr int kRepeats = 200;

// Per-rank mean time of one Allreduce. Every rank times the same collective,
// so the caller averages these values across the world for a stable number.
double bench_allreduce(const std::vector<double>& send, std::vector<double>& recv, int count) {
    for (int i = 0; i < kWarmup; ++i) {
        MPI_Allreduce(send.data(), recv.data(), count, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    for (int i = 0; i < kRepeats; ++i) {
        MPI_Allreduce(send.data(), recv.data(), count, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    }
    return (MPI_Wtime() - start) / kRepeats;
}

}  // namespace

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    constexpr int kMaxElements = 1 << 20;  // 8 MiB at double
    std::vector<double> send(kMaxElements, 1.0), recv(kMaxElements, 0.0);

    if (rank == 0) {
        std::printf("MPI_Allreduce SUM on %d ranks (%d warmup, %d timed rounds)\n", size, kWarmup,
                    kRepeats);
        std::printf("%12s | %12s | %12s\n", "elements", "time", "bandwidth");
        std::printf("-------------|--------------|-------------\n");
    }

    for (int count = 1; count <= kMaxElements; count *= 4) {
        const double elapsed = bench_allreduce(send, recv, count);

        // Rank 0 averages per-rank timings over the world.
        double mean = 0.0;
        MPI_Allreduce(&elapsed, &mean, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        mean /= size;

        if (rank == 0) {
            const double bytes = static_cast<double>(count) * sizeof(double);
            const double bandwidth = bytes / mean / (1024.0 * 1024.0);
            std::printf("%12d | %9.3f us | %8.1f MiB/s\n", count, mean * 1e6, bandwidth);
        }
    }

    // Correctness sentinel: sum of `size` copies of 1.0 on the first slot.
    if (recv[0] != static_cast<double>(size)) {
        std::fprintf(stderr, "rank %d: allreduce produced wrong sum %f\n", rank, recv[0]);
        MPI_Finalize();
        return 1;
    }

    MPI_Finalize();
    return 0;
}
