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
 */

#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

namespace hpc::memory {

//------------------------------------------------------------------------------
// Array of Structures (AOS) - Traditional approach
//------------------------------------------------------------------------------

/**
 * @brief Particle stored as a single structure
 * 
 * Memory layout: [x,y,z,vx,vy,vz][x,y,z,vx,vy,vz][x,y,z,vx,vy,vz]...
 * 
 * When updating only positions, we still load velocity data into cache,
 * wasting cache space and memory bandwidth.
 */
struct ParticleAOS {
    float x, y, z;      // Position
    float vx, vy, vz;   // Velocity
};

/**
 * @brief Update particle positions using AOS layout
 * 
 * For each particle, we access x, y, z, vx, vy, vz which are contiguous
 * in memory for that particle, but the next particle's data may not be
 * in cache.
 */
void update_particles_aos(std::vector<ParticleAOS>& particles, float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

/**
 * @brief Compute total kinetic energy using AOS layout
 */
float compute_energy_aos(const std::vector<ParticleAOS>& particles) {
    float energy = 0.0f;
    for (const auto& p : particles) {
        energy += p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
    }
    return 0.5f * energy;
}

//------------------------------------------------------------------------------
// Structure of Arrays (SOA) - Cache-friendly approach
//------------------------------------------------------------------------------

/**
 * @brief Particles stored as separate arrays for each field
 * 
 * Memory layout:
 * x:  [x0, x1, x2, x3, ...]
 * y:  [y0, y1, y2, y3, ...]
 * z:  [z0, z1, z2, z3, ...]
 * vx: [vx0, vx1, vx2, vx3, ...]
 * vy: [vy0, vy1, vy2, vy3, ...]
 * vz: [vz0, vz1, vz2, vz3, ...]
 * 
 * When updating positions, we only load position and velocity data,
 * maximizing cache utilization. Also enables SIMD vectorization.
 */
struct ParticleSOA {
    std::vector<float> x, y, z;      // Positions
    std::vector<float> vx, vy, vz;   // Velocities
    
    void resize(size_t n) {
        x.resize(n);
        y.resize(n);
        z.resize(n);
        vx.resize(n);
        vy.resize(n);
        vz.resize(n);
    }
    
    size_t size() const { return x.size(); }
};

/**
 * @brief Update particle positions using SOA layout
 * 
 * Each loop accesses contiguous memory, maximizing cache efficiency.
 * The compiler can also auto-vectorize these loops easily.
 */
void update_particles_soa(ParticleSOA& particles, float dt) {
    const size_t n = particles.size();
    
    // These loops are easily vectorizable
    for (size_t i = 0; i < n; ++i) {
        particles.x[i] += particles.vx[i] * dt;
    }
    for (size_t i = 0; i < n; ++i) {
        particles.y[i] += particles.vy[i] * dt;
    }
    for (size_t i = 0; i < n; ++i) {
        particles.z[i] += particles.vz[i] * dt;
    }
}

/**
 * @brief Compute total kinetic energy using SOA layout
 */
float compute_energy_soa(const ParticleSOA& particles) {
    float energy = 0.0f;
    const size_t n = particles.size();
    
    for (size_t i = 0; i < n; ++i) {
        energy += particles.vx[i] * particles.vx[i];
    }
    for (size_t i = 0; i < n; ++i) {
        energy += particles.vy[i] * particles.vy[i];
    }
    for (size_t i = 0; i < n; ++i) {
        energy += particles.vz[i] * particles.vz[i];
    }
    
    return 0.5f * energy;
}

//------------------------------------------------------------------------------
// Initialization helpers
//------------------------------------------------------------------------------

void initialize_aos(std::vector<ParticleAOS>& particles, size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    particles.resize(n);
    for (auto& p : particles) {
        p.x = dist(rng);
        p.y = dist(rng);
        p.z = dist(rng);
        p.vx = dist(rng);
        p.vy = dist(rng);
        p.vz = dist(rng);
    }
}

void initialize_soa(ParticleSOA& particles, size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    particles.resize(n);
    for (size_t i = 0; i < n; ++i) {
        particles.x[i] = dist(rng);
        particles.y[i] = dist(rng);
        particles.z[i] = dist(rng);
        particles.vx[i] = dist(rng);
        particles.vy[i] = dist(rng);
        particles.vz[i] = dist(rng);
    }
}

} // namespace hpc::memory

//------------------------------------------------------------------------------
// Demo main
//------------------------------------------------------------------------------

int main() {
    using namespace hpc::memory;
    using namespace std::chrono;
    
    constexpr size_t NUM_PARTICLES = 1'000'000;
    constexpr int NUM_ITERATIONS = 100;
    constexpr float DT = 0.01f;
    
    std::cout << "=== AOS vs SOA Performance Comparison ===\n";
    std::cout << "Particles: " << NUM_PARTICLES << "\n";
    std::cout << "Iterations: " << NUM_ITERATIONS << "\n\n";
    
    // AOS benchmark
    {
        std::vector<ParticleAOS> particles;
        initialize_aos(particles, NUM_PARTICLES);
        
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
        initialize_soa(particles, NUM_PARTICLES);
        
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
    
    return 0;
}
