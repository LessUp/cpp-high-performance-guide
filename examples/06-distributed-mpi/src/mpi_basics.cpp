/**
 * @file mpi_basics.cpp
 * @brief MPI execution model basics: init/finalize, ranks, point-to-point.
 *
 * MPI programs run in the SPMD model: the same binary starts on every rank,
 * and ranks tell themselves apart via MPI_Comm_rank(). Communication is
 * explicit — there is no shared memory between ranks (even when they live
 * on the same node, data still crosses a message boundary).
 *
 * Run with:  mpirun -np 4 ./mpi_basics
 * A single rank (np=1) also works and is used as the CI smoke test.
 */

#include <mpi.h>

#include <cstdio>
#include <vector>

namespace {

// Ring hello: each rank sends its id around the ring so output is ordered
// rank 0 -> 1 -> ... -> size-1 -> back to 0. Point-to-point Send/Recv also
// doubles as the first correctness check of the payload.
int ring_hello(int rank, int size) {
    const int send_to = (rank + 1) % size;
    const int recv_from = (rank - 1 + size) % size;

    const int payload = rank;
    int received = -1;

    if (rank == 0) {
        MPI_Send(&payload, 1, MPI_INT, send_to, /*tag=*/0, MPI_COMM_WORLD);
        MPI_Recv(&received, 1, MPI_INT, recv_from, /*tag=*/0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(&received, 1, MPI_INT, recv_from, /*tag=*/0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&payload, 1, MPI_INT, send_to, /*tag=*/0, MPI_COMM_WORLD);
    }

    char name[MPI_MAX_PROCESSOR_NAME] = {};
    int name_len = 0;
    MPI_Get_processor_name(name, &name_len);
    std::printf("[rank %d/%d on %.64s] received payload %d from rank %d\n", rank, size, name,
                received, recv_from);

    return received == recv_from ? 0 : 1;
}

// Bulk transfer check: rank 0 scatters a pattern to every rank directly with
// Send/Recv pairs so each rank can verify a larger payload.
int bulk_transfer_check(int rank, int size) {
    constexpr int kElements = 1024;
    std::vector<int> buffer(kElements, -1);

    if (rank == 0) {
        for (int dest = 1; dest < size; ++dest) {
            std::vector<int> out(kElements, dest * 7);
            MPI_Send(out.data(), kElements, MPI_INT, dest, /*tag=*/1, MPI_COMM_WORLD);
        }
        buffer.assign(kElements, 0);  // rank 0 keeps its own pattern
    } else {
        MPI_Recv(buffer.data(), kElements, MPI_INT, 0, /*tag=*/1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    const int expected = rank * 7;
    for (int value : buffer) {
        if (value != expected) {
            std::fprintf(stderr, "rank %d: payload corrupted (got %d, want %d)\n", rank, value,
                         expected);
            return 1;
        }
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int failures = 0;
    failures += ring_hello(rank, size);

    MPI_Barrier(MPI_COMM_WORLD);

    failures += bulk_transfer_check(rank, size);

    // All-reduce the failure count so every rank knows the global verdict —
    // a common pattern for asserting distributed test results.
    int global_failures = 0;
    MPI_Allreduce(&failures, &global_failures, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0) {
        std::printf("mpi_basics: world size %d, %s\n", size,
                    global_failures == 0 ? "OK" : "FAILED");
    }

    MPI_Finalize();
    return global_failures == 0 ? 0 : 1;
}
