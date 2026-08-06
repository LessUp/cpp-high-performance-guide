/**
 * @file collectives.cpp
 * @brief The five workhorse collectives: Bcast, Scatter, Gather, Reduce,
 *        Allreduce.
 *
 * Collectives involve ALL ranks of a communicator and are implemented on top
 * of optimized topology-aware algorithms (trees, butterflies, recursive
 * halving/doubling) inside the MPI library — hand-rolling them with
 * point-to-point calls is a classic performance mistake.
 *
 * Run with:  mpirun -np 4 ./collectives
 */

#include <mpi.h>

#include <cmath>
#include <cstdio>
#include <vector>

namespace {

int check_broadcast(int rank) {
    // Bcast: one root distributes, everyone else receives. The root's buffer
    // is unchanged; all other ranks get overwritten.
    int value = (rank == 0) ? 42 : -1;
    MPI_Bcast(&value, 1, MPI_INT, /*root=*/0, MPI_COMM_WORLD);
    return value == 42 ? 0 : 1;
}

int check_scatter_gather(int rank, int size) {
    // Scatter splits root's array into equal chunks, one per rank; Gather is
    // the inverse. Here each rank squares its chunk, so the gathered result
    // must be the elementwise square of the original input.
    constexpr int kPerRank = 64;
    const int total = kPerRank * size;

    std::vector<double> global(static_cast<std::size_t>(total), 0.0);
    std::vector<double> chunk(static_cast<std::size_t>(kPerRank), 0.0);

    if (rank == 0) {
        for (int i = 0; i < total; ++i) {
            global[static_cast<std::size_t>(i)] = static_cast<double>(i) * 0.5;
        }
    }

    MPI_Scatter(global.data(), kPerRank, MPI_DOUBLE, chunk.data(), kPerRank, MPI_DOUBLE,
                /*root=*/0, MPI_COMM_WORLD);

    for (int i = 0; i < kPerRank; ++i) {
        chunk[static_cast<std::size_t>(i)] *= chunk[static_cast<std::size_t>(i)];
    }

    std::vector<double> gathered(static_cast<std::size_t>(total), 0.0);
    MPI_Gather(chunk.data(), kPerRank, MPI_DOUBLE, gathered.data(), kPerRank, MPI_DOUBLE,
               /*root=*/0, MPI_COMM_WORLD);

    if (rank != 0) {
        return 0;
    }
    for (int i = 0; i < total; ++i) {
        const double x = static_cast<double>(i) * 0.5;
        if (std::fabs(gathered[static_cast<std::size_t>(i)] - x * x) > 1e-12) {
            return 1;
        }
    }
    return 0;
}

int check_reduce(int rank, int size) {
    // Reduce: combines one value per rank at the root. Sum of ranks
    // 0..size-1 must equal size*(size-1)/2.
    const int contribution = rank;
    int sum_at_root = -1;
    MPI_Reduce(&contribution, &sum_at_root, 1, MPI_INT, MPI_SUM, /*root=*/0, MPI_COMM_WORLD);

    if (rank != 0) {
        return 0;
    }
    return sum_at_root == size * (size - 1) / 2 ? 0 : 1;
}

int check_allreduce_dot(int rank, int size) {
    // The most common HPC collective: a global dot product. Every rank
    // computes its slice locally, Allreduce combines the partial sums, and
    // EVERY rank ends up with the full result (unlike Reduce).
    constexpr int kPerRank = 256;
    const int total = kPerRank * size;
    const std::size_t base = static_cast<std::size_t>(rank) * kPerRank;

    double partial = 0.0;
    for (int i = 0; i < kPerRank; ++i) {
        const double g = static_cast<double>(base + static_cast<std::size_t>(i));
        // a[i] = 1, b[i] = g  =>  dot = sum of 0..total-1
        partial += g;
    }

    double dot = 0.0;
    MPI_Allreduce(&partial, &dot, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    const double expected = static_cast<double>(total) * (total - 1) / 2.0;
    return std::fabs(dot - expected) < 1e-6 * expected ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct Case {
        const char* name;
        int (*fn)(int, int);
    };
    const Case cases[] = {
        {"Bcast", [](int r, int) { return check_broadcast(r); }},
        {"Scatter/Gather", check_scatter_gather},
        {"Reduce", check_reduce},
        {"Allreduce (dot product)", check_allreduce_dot},
    };

    int failures = 0;
    for (const Case& c : cases) {
        int local = c.fn(rank, size);
        int global = 0;
        MPI_Allreduce(&local, &global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (rank == 0) {
            std::printf("%-28s %s\n", c.name, global == 0 ? "OK" : "FAILED");
        }
        failures += (global != 0);
    }

    MPI_Finalize();
    return failures == 0 ? 0 : 1;
}
