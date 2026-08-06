# Distributed-memory (MPI) examples
#
# This subdirectory is only entered when HPC_ENABLE_MPI=ON and an MPI
# implementation was found; see the top-level CMakeLists.txt.

hpc_add_example(
    NAME mpi_basics
    SOURCES src/mpi_basics.cpp
    LIBRARIES MPI::MPI_CXX
)

hpc_add_example(
    NAME mpi_collectives
    SOURCES src/collectives.cpp
    LIBRARIES MPI::MPI_CXX
)

hpc_add_example(
    NAME mpi_nonblocking_overlap
    SOURCES src/nonblocking_overlap.cpp
    LIBRARIES MPI::MPI_CXX
)

hpc_add_example(
    NAME mpi_jacobi_1d
    SOURCES src/jacobi_1d.cpp
    LIBRARIES MPI::MPI_CXX
)

# Communication benchmarks use manual MPI_Wtime() loops instead of Google
# Benchmark: the benchmark library controls iteration counts per process,
# which can deadlock paired/collective communication. They are plain
# executables named *_bench for consistency with the rest of the repo.
hpc_add_example(
    NAME mpi_pingpong_bench
    SOURCES bench/mpi_pingpong_bench.cpp
    LIBRARIES MPI::MPI_CXX
)

hpc_add_example(
    NAME mpi_allreduce_bench
    SOURCES bench/mpi_allreduce_bench.cpp
    LIBRARIES MPI::MPI_CXX
)

# ---------------------------------------------------------------------------
# Smoke tests. Single-rank runs exercise the full code path (MPI is valid
# with world size 1) and need no oversubscription, so they are safe for any
# machine. Multi-rank runs are registered only when enough cores exist,
# because Open MPI refuses to oversubscribe by default.
# ---------------------------------------------------------------------------
set(_hpc_mpi_single_rank_targets mpi_basics mpi_collectives mpi_nonblocking_overlap mpi_jacobi_1d)

foreach(target ${_hpc_mpi_single_rank_targets})
    add_test(NAME ${target}_np1
        COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 1 $<TARGET_FILE:${target}>
    )
    set_tests_properties(${target}_np1 PROPERTIES LABELS "mpi")
endforeach()

include(ProcessorCount)
ProcessorCount(_hpc_cpu_count)

if(_hpc_cpu_count GREATER_EQUAL 4)
    foreach(target mpi_collectives mpi_nonblocking_overlap mpi_jacobi_1d)
        add_test(NAME ${target}_np4
            COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 4 $<TARGET_FILE:${target}>
        )
        set_tests_properties(${target}_np4 PROPERTIES LABELS "mpi")
    endforeach()
endif()
