/**
 * @file aos_vs_soa.cpp
 * @brief Array of Structures (AOS) vs Structure of Arrays (SOA) comparison
 *
 * This example demonstrates the performance difference between AOS and SOA
 * data layouts. SOA is typically faster for sequential access patterns because
 * it improves cache utilization - accessing one field of all elements keeps
 * the data in contiguous memory.
 *
 * Key concepts:
 * - Data locality and cache efficiency
 * - Memory access patterns
 * - When to use AOS vs SOA
 *
 * Validates:
 *   - Requirement 2.1: AOS vs SOA Comparison
 */

#include <chrono>
#include <hpc/particle_types.hpp>
#include <iostream>

namespace {

using namespace hpc::memory;
using namespace std::chrono;

void run_demo() {
    constexpr size_t NUM_PARTICLES = 1'000'000;
    constexpr int NUM_ITERATIONS = 100;
    constexpr float DT = 0.01f;

    std::cout << "=== AOS vs SOA Performance Comparison ===\n";
    std::cout << "Particles: " << NUM_PARTICLES << "\n";
    std::cout << "Iterations: " << NUM_ITERATIONS << "\n\n";

    // AOS benchmark
    {
        std::vector<ParticleAOS> particles;
        initialize_particles(particles, NUM_PARTICLES);

        auto start = high_resolution_clock::now();
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            update_particles_aos(particles, DT);
        }
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start).count();
        std::cout << "AOS update time: " << duration << " ms\n";
        std::cout << "AOS energy: " << compute_energy_aos(particles) << "\n\n";
    }

    // SOA benchmark
    {
        ParticleSOA particles;
        initialize_particles(particles, NUM_PARTICLES);

        auto start = high_resolution_clock::now();
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            update_particles_soa(particles, DT);
        }
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start).count();
        std::cout << "SOA update time: " << duration << " ms\n";
        std::cout << "SOA energy: " << compute_energy_soa(particles) << "\n\n";
    }

    std::cout << "Note: SOA should be faster due to better cache utilization.\n";
    std::cout << "The compiler can also auto-vectorize SOA loops more easily.\n";
}

}  // namespace

int main() {
    run_demo();
    return 0;
}
