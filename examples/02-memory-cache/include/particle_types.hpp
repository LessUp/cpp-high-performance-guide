/**
 * @file particle_types.hpp
 * @brief Particle data structures for AOS vs SOA comparison
 *
 * Validates:
 *   - Requirement 2.1: AOS vs SOA Comparison
 *
 * This header defines particle data structures used to demonstrate
 * the performance difference between Array of Structures (AOS) and
 * Structure of Arrays (SOA) memory layouts.
 */

#pragma once

#include <cstddef>
#include <vector>

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
    float x, y, z;     // Position
    float vx, vy, vz;  // Velocity
};

/**
 * @brief Update particle positions using AOS layout
 *
 * For each particle, we access x, y, z, vx, vy, vz which are contiguous
 * in memory for that particle, but the next particle's data may not be
 * in cache.
 */
inline void update_particles_aos(std::vector<ParticleAOS>& particles, float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

/**
 * @brief Compute total kinetic energy using AOS layout
 */
inline float compute_energy_aos(const std::vector<ParticleAOS>& particles) {
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
    std::vector<float> x, y, z;     // Positions
    std::vector<float> vx, vy, vz;  // Velocities

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
inline void update_particles_soa(ParticleSOA& particles, float dt) {
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
inline float compute_energy_soa(const ParticleSOA& particles) {
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

/**
 * @brief Initialize AOS particles with deterministic random values
 */
inline void initialize_particles(std::vector<ParticleAOS>& particles, size_t n) {
    // 使用确定性初始化而非随机，避免 <random> 依赖
    particles.resize(n);
    for (size_t i = 0; i < n; ++i) {
        float fi = static_cast<float>(i);
        particles[i].x = fi * 0.001f;
        particles[i].y = fi * 0.002f;
        particles[i].z = fi * 0.003f;
        particles[i].vx = 1.0f;
        particles[i].vy = 1.0f;
        particles[i].vz = 1.0f;
    }
}

/**
 * @brief Initialize SOA particles with deterministic random values
 */
inline void initialize_particles(ParticleSOA& particles, size_t n) {
    particles.resize(n);
    for (size_t i = 0; i < n; ++i) {
        float fi = static_cast<float>(i);
        particles.x[i] = fi * 0.001f;
        particles.y[i] = fi * 0.002f;
        particles.z[i] = fi * 0.003f;
        particles.vx[i] = 1.0f;
        particles.vy[i] = 1.0f;
        particles.vz[i] = 1.0f;
    }
}

}  // namespace hpc::memory
