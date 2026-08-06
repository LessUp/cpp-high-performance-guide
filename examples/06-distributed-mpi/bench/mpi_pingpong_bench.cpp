/**
 * @file mpi_pingpong_bench.cpp
 * @brief Point-to-point latency/bandwidth ping-pong between rank 0 and 1.
 *
 * Deliberately NOT built on Google Benchmark: the benchmark library decides
 * iteration counts independently per process, which can deadlock when every
 * iteration contains a collective/paired communication. MPI communication
 * benchmarks therefore use manual timing loops with MPI_Wtime().
 *
 * Run with:  mpirun -np 2 ./mpi_pingpong_bench
 */

#include <mpi.h>

#include <cstdio>
#include <vector>

namespace {

constexpr int kWarmup = 10;
constexpr int kRepeats = 100;

// Half round-trip time per message size. Rank 0 sends then receives; rank 1
// mirrors. Returns latency in seconds for one one-way hop.
double ping_pong(std::vector<char>& buffer, int size_bytes, int rank) {
    for (int i = 0; i < kWarmup; ++i) {
        if (rank == 0) {
            MPI_Send(buffer.data(), size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer.data(), size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(buffer.data(), size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer.data(), size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    for (int i = 0; i < kRepeats; ++i) {
        if (rank == 0) {
            MPI_Send(buffer.data(), size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer.data(), size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(buffer.data(), size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer.data(), size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }
    const double elapsed = MPI_Wtime() - start;
    return elapsed / (2.0 * kRepeats);  // one round trip = two hops
}

}  // namespace

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            std::printf("mpi_pingpong_bench needs at least 2 ranks (mpirun -np 2). Skipping.\n");
        }
        MPI_Finalize();
        return 0;
    }

    constexpr int kMaxBytes = 4 * 1024 * 1024;
    std::vector<char> buffer(kMaxBytes, static_cast<char>(rank));

    if (rank == 0) {
        std::printf("MPI ping-pong between rank 0 and 1 (%d warmup, %d timed rounds)\n", kWarmup,
                    kRepeats);
        std::printf("%12s | %12s | %12s\n", "size", "latency", "bandwidth");
        std::printf("-------------|--------------|-------------\n");
    }

    for (int bytes = 1; bytes <= kMaxBytes; bytes *= 4) {
        const double latency = ping_pong(buffer, bytes, rank);
        if (rank == 0) {
            const double bandwidth = static_cast<double>(bytes) / latency / (1024.0 * 1024.0);
            if (bytes < 1024) {
                std::printf("%10d B | %9.3f us | %8.1f MiB/s\n", bytes, latency * 1e6, bandwidth);
            } else if (bytes < 1024 * 1024) {
                std::printf("%9d KiB | %9.3f us | %8.1f MiB/s\n", bytes / 1024, latency * 1e6,
                            bandwidth);
            } else {
                std::printf("%9d MiB | %9.3f us | %8.1f MiB/s\n", bytes / (1024 * 1024),
                            latency * 1e6, bandwidth);
            }
        }
    }

    MPI_Finalize();
    return 0;
}
