/**
 * @file particle_types_test.cpp
 * @brief Unit tests for particle_types.hpp (AOS vs SOA equivalence)
 */

#include <gtest/gtest.h>

#include <cstddef>
#include <hpc/particle_types.hpp>
#include <vector>

namespace hpc::memory::test {

namespace {

constexpr size_t kParticleCount = 257;  // Deliberately not a round number
constexpr float kDt = 0.01f;

// Deterministic, non-trivial initial state shared by the AOS and SOA
// fixtures, so both layouts start from bit-identical values.
float init_x(size_t i) {
    return static_cast<float>(i) * 0.05f - 3.0f;
}
float init_y(size_t i) {
    return static_cast<float>(i % 13) * 0.25f;
}
float init_z(size_t i) {
    return -static_cast<float>(i) * 0.0125f + 1.0f;
}
float init_vx(size_t i) {
    return static_cast<float>(i % 7) - 3.0f;
}
float init_vy(size_t i) {
    return static_cast<float>(i % 5) * 0.5f - 1.0f;
}
float init_vz(size_t i) {
    return static_cast<float>(i % 11) * -0.125f;
}

void make_aos(std::vector<ParticleAOS>& particles) {
    particles.resize(kParticleCount);
    for (size_t i = 0; i < kParticleCount; ++i) {
        particles[i].x = init_x(i);
        particles[i].y = init_y(i);
        particles[i].z = init_z(i);
        particles[i].vx = init_vx(i);
        particles[i].vy = init_vy(i);
        particles[i].vz = init_vz(i);
    }
}

void make_soa(ParticleSOA& particles) {
    particles.resize(kParticleCount);
    for (size_t i = 0; i < kParticleCount; ++i) {
        particles.x[i] = init_x(i);
        particles.y[i] = init_y(i);
        particles.z[i] = init_z(i);
        particles.vx[i] = init_vx(i);
        particles.vy[i] = init_vy(i);
        particles.vz[i] = init_vz(i);
    }
}

// Reference kinetic energy: the header uses unit mass (no mass field), i.e.
// 0.5 * sum(vx^2 + vy^2 + vz^2). Accumulated in double so the reference is
// independent of the float implementations under test.
double reference_energy() {
    double energy = 0.0;
    for (size_t i = 0; i < kParticleCount; ++i) {
        const double vx = static_cast<double>(init_vx(i));
        const double vy = static_cast<double>(init_vy(i));
        const double vz = static_cast<double>(init_vz(i));
        energy += 0.5 * (vx * vx + vy * vy + vz * vz);
    }
    return energy;
}

}  // namespace

// ---------------------------------------------------------------------------
// Position integration: AOS and SOA must produce identical states
// ---------------------------------------------------------------------------

TEST(ParticleTypesTest, UpdateAosAndSoaProduceEquivalentStates) {
    std::vector<ParticleAOS> aos;
    ParticleSOA soa;
    make_aos(aos);
    make_soa(soa);

    update_particles_aos(aos, kDt);
    update_particles_soa(soa, kDt);

    ASSERT_EQ(aos.size(), soa.size());
    ASSERT_EQ(aos.size(), kParticleCount);
    for (size_t i = 0; i < kParticleCount; ++i) {
        EXPECT_NEAR(aos[i].x, soa.x[i], 1e-5f);
        EXPECT_NEAR(aos[i].y, soa.y[i], 1e-5f);
        EXPECT_NEAR(aos[i].z, soa.z[i], 1e-5f);
        // Velocities are not modified by the update, but compare every field.
        EXPECT_NEAR(aos[i].vx, soa.vx[i], 1e-5f);
        EXPECT_NEAR(aos[i].vy, soa.vy[i], 1e-5f);
        EXPECT_NEAR(aos[i].vz, soa.vz[i], 1e-5f);
    }
}

// ---------------------------------------------------------------------------
// Kinetic energy: AOS and SOA agree with each other and with the reference
// ---------------------------------------------------------------------------

TEST(ParticleTypesTest, EnergyAosAndSoaAgreeWithReference) {
    std::vector<ParticleAOS> aos;
    ParticleSOA soa;
    make_aos(aos);
    make_soa(soa);

    const float energy_aos = compute_energy_aos(aos);
    const float energy_soa = compute_energy_soa(soa);

    // AOS and SOA accumulate in different orders; allow a small tolerance
    // for float rounding even though the values coincide on this dataset.
    EXPECT_NEAR(energy_aos, energy_soa, 1e-3f);
    EXPECT_NEAR(energy_aos, static_cast<float>(reference_energy()), 1e-3f);
    EXPECT_NEAR(energy_soa, static_cast<float>(reference_energy()), 1e-3f);
}

}  // namespace hpc::memory::test
