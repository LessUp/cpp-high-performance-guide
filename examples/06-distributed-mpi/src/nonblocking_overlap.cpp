/**
 * @file nonblocking_overlap.cpp
 * @brief Non-blocking communication (Isend/Irecv) to overlap halo
 *        exchange with interior computation.
 *
 * Blocking Send/Recv stalls the CPU until the message is done; non-blocking
 * calls return immediately with an MPI_Request, letting the rank compute on
 * data it already owns while the NIC moves the halo in the background.
 * This is the standard recipe for stencil codes.
 *
 * Layout per rank (1D domain decomposition):
 *
 *     [ halo_left | ....... owned interior ....... | halo_right ]
 *
 * Run with:  mpirun -np 4 ./nonblocking_overlap
 */

#include <mpi.h>

#include <cstdio>
#include <numeric>
#include <vector>

namespace {

constexpr int kLocal = 512;  // owned cells per rank
constexpr int kIterations = 50;

// One halo-exchange round using non-blocking calls. Interior cells
// [1, kLocal] are updated from the previous step while the halo transfers
// are in flight; halos are only read after MPI_Waitall.
int run_overlap_round(std::vector<double>& buf, int rank, int size) {
    // buf layout: index 0 = left halo, 1..kLocal = owned, kLocal+1 = right halo
    const int left = rank - 1;
    const int right = rank + 1;
    const bool has_left = rank > 0;
    const bool has_right = rank < size - 1;

    std::vector<MPI_Request> requests;
    requests.reserve(4);

    // Post receives first so incoming data has somewhere to land.
    if (has_left) {
        requests.emplace_back();
        MPI_Irecv(&buf[0], 1, MPI_DOUBLE, left, /*tag=*/0, MPI_COMM_WORLD, &requests.back());
    }
    if (has_right) {
        requests.emplace_back();
        MPI_Irecv(&buf[kLocal + 1], 1, MPI_DOUBLE, right, /*tag=*/0, MPI_COMM_WORLD,
                  &requests.back());
    }

    // Send our boundary values to the neighbours.
    if (has_left) {
        requests.emplace_back();
        MPI_Isend(&buf[1], 1, MPI_DOUBLE, left, /*tag=*/0, MPI_COMM_WORLD, &requests.back());
    }
    if (has_right) {
        requests.emplace_back();
        MPI_Isend(&buf[kLocal], 1, MPI_DOUBLE, right, /*tag=*/0, MPI_COMM_WORLD, &requests.back());
    }

    // Overlap window: update the interior without touching halos.
    // (A real stencil would read buf[i-1], buf[i], buf[i+1] here; we do a
    // cheap smoothing pass so the example stays self-contained.)
    double sink = 0.0;
    for (int i = 2; i < kLocal; ++i) {
        sink += buf[static_cast<std::size_t>(i)] * 0.5;
    }
    // Never-true guard keeps the loop alive under -O3 without disturbing the
    // communicated boundary cells buf[1] / buf[kLocal].
    if (sink == 12345.678) {
        buf[static_cast<std::size_t>(kLocal) / 2] = sink;
    }

    // Drain the exchanges before anyone reads a halo cell.
    if (!requests.empty()) {
        MPI_Waitall(static_cast<int>(requests.size()), requests.data(), MPI_STATUSES_IGNORE);
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

    // Owned cells start at rank+1 so a correct halo exchange delivers the
    // neighbour's rank value, which we can then assert on.
    std::vector<double> buf(static_cast<std::size_t>(kLocal) + 2, static_cast<double>(rank + 1));

    int failures = 0;
    for (int it = 0; it < kIterations; ++it) {
        failures += run_overlap_round(buf, rank, size);
    }

    // Verify the halos: left halo must hold the left neighbour's value and
    // vice versa. Boundary ranks keep their initial sentinel on the open side.
    const double expect_left = rank > 0 ? static_cast<double>(rank) : static_cast<double>(rank + 1);
    const double expect_right =
        rank < size - 1 ? static_cast<double>(rank + 2) : static_cast<double>(rank + 1);
    if (buf[0] != expect_left) {
        std::fprintf(stderr, "rank %d: left halo %f, want %f\n", rank, buf[0], expect_left);
        ++failures;
    }
    if (buf[kLocal + 1] != expect_right) {
        std::fprintf(stderr, "rank %d: right halo %f, want %f\n", rank, buf[kLocal + 1],
                     expect_right);
        ++failures;
    }

    int global_failures = 0;
    MPI_Allreduce(&failures, &global_failures, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (rank == 0) {
        std::printf("nonblocking_overlap: %d iterations, %d ranks, %s\n", kIterations, size,
                    global_failures == 0 ? "OK" : "FAILED");
    }

    MPI_Finalize();
    return global_failures == 0 ? 0 : 1;
}
