/**
 * @file data_structures_bench.cpp
 * @brief Lookup benchmark: std::map vs std::unordered_map vs sorted vector.
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <map>
#include <random>
#include <unordered_map>
#include <vector>

namespace {

struct Fixture {
    std::vector<int> probes;
    std::map<int, int> tree;
    std::unordered_map<int, int> hash;
    std::vector<std::pair<int, int>> sorted_vec;

    explicit Fixture(int entries) {
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, entries * 2);

        probes.resize(100'000);
        for (int& p : probes) {
            p = dist(rng);
        }
        for (int i = 0; i < entries; ++i) {
            const int k = dist(rng);
            tree.emplace(k, k * 3);
            hash.emplace(k, k * 3);
            sorted_vec.emplace_back(k, k * 3);
        }
        std::sort(sorted_vec.begin(), sorted_vec.end());
    }
};

Fixture& fixture(int entries) {
    static std::map<int, Fixture*> cache;
    auto it = cache.find(entries);
    if (it == cache.end()) {
        it = cache.emplace(entries, new Fixture(entries)).first;
    }
    return *it->second;
}

}  // namespace

static void BM_LookupStdMap(benchmark::State& state) {
    Fixture& f = fixture(static_cast<int>(state.range(0)));
    for (auto _ : state) {
        long long sum = 0;
        for (int p : f.probes) {
            const auto it = f.tree.find(p);
            if (it != f.tree.end()) {
                sum += it->second;
            }
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * f.probes.size());
}

static void BM_LookupUnorderedMap(benchmark::State& state) {
    Fixture& f = fixture(static_cast<int>(state.range(0)));
    for (auto _ : state) {
        long long sum = 0;
        for (int p : f.probes) {
            const auto it = f.hash.find(p);
            if (it != f.hash.end()) {
                sum += it->second;
            }
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * f.probes.size());
}

static void BM_LookupSortedVector(benchmark::State& state) {
    Fixture& f = fixture(static_cast<int>(state.range(0)));
    for (auto _ : state) {
        long long sum = 0;
        for (int p : f.probes) {
            const auto it =
                std::lower_bound(f.sorted_vec.begin(), f.sorted_vec.end(), std::make_pair(p, 0),
                                 [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                                     return a.first < b.first;
                                 });
            if (it != f.sorted_vec.end() && it->first == p) {
                sum += it->second;
            }
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * f.probes.size());
}

BENCHMARK(BM_LookupStdMap)->Arg(10'000)->Arg(100'000);
BENCHMARK(BM_LookupUnorderedMap)->Arg(10'000)->Arg(100'000);
BENCHMARK(BM_LookupSortedVector)->Arg(10'000)->Arg(100'000);
