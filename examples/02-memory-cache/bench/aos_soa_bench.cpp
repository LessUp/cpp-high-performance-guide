/**
 * @file aos_soa_bench.cpp
 * @brief Benchmark comparing AOS vs SOA performance
 * 
 * Property 3: SOA Performance Advantage for Sequential Access
 * Validates: Requirements 2.1
 */

#include <benchmark/benchmark.h>
#include <vector>
#include <random>

namespace {

//------------------------------------------------------------------------------
// AOS Implementation
//------------------------------------------------------------------------------

struct ParticleAOS {
    float x, y, z;
    float vx, vy, vz;
};

void update_aos(std::vector<ParticleAOS>& particles, float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

//------------------------------------------------------------------------------
// SOA Implementation
//------------------------------------------------------------------------------

struct ParticleSOA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    
    void resize(size_t n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
    }
    
    size_t size() const { return x.size(); }
};

void update_soa(ParticleSOA& particles, float dt) {
    const size_t n = particles.size();
    for (size_t i = 0; i < n; ++i) particles.x[i] += particles.vx[i] * dt;
    for (size_t i = 0; i < n; ++i) particles.y[i] += particles.vy[i] * dt;
    for (size_t i = 0; i < n; ++i) particles.z[i] += particles.vz[i] * dt;
}

//------------------------------------------------------------------------------
// Initialization
//------------------------------------------------------------------------------

void init_aos(std::vector<ParticleAOS>& p, size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    p.resize(n);
    for (auto& particle : p) {
        particle.x = dist(rng); particle.y = dist(rng); particle.z = dist(rng);
        particle.vx = dist(rng); particle.vy = dist(rng); particle.vz = dist(rng);
    }
}

void init_soa(ParticleSOA& p, size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    p.resize(n);
    for (size_t i = 0; i < n; ++i) {
        p.x[i] = dist(rng); p.y[i] = dist(rng); p.z[i] = dist(rng);
        p.vx[i] = dist(rng); p.vy[i] = dist(rng); p.vz[i] = dist(rng);
    }
}

//------------------------------------------------------------------------------
// Benchmarks
//------------------------------------------------------------------------------

static void BM_AOS_Update(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    std::vector<ParticleAOS> particles;
    init_aos(particles, n);
    
    for (auto _ : state) {
        update_aos(particles, 0.01f);
        benchmark::DoNotOptimize(particles.data());
        benchmark::ClobberMemory();
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(n * sizeof(ParticleAOS)));
}

static void BM_SOA_Update(benchmark::State& state) {
    const size_t n = static_cast<size_t>(state.range(0));
    ParticleSOA particles;
    init_soa(particles, n);
    
    for (auto _ : state) {
        update_soa(particles, 0.01f);
        benchmark::DoNotOptimize(particles.x.data());
        benchmark::ClobberMemory();
    }
    
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(n * 6 * sizeof(float)));
}

// Register benchmarks with different sizes
BENCHMARK(BM_AOS_Update)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_SOA_Update)
    ->RangeMultiplier(4)
    ->Range(1024, 4 * 1024 * 1024)
    ->Unit(benchmark::kMicrosecond);

} // namespace

BENCHMARK_MAIN();
