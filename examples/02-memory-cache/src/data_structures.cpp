/**
 * @file data_structures.cpp
 * @brief Cache-friendliness of lookup structures: std::map vs
 *        std::unordered_map vs sorted vector + binary search.
 *
 * Asymptotics is not the whole story for lookups: a red-black tree does
 * O(log n) pointer chases, each likely a cache miss; a sorted vector does
 * O(log n) probes into contiguous memory; a hash table does one hash plus a
 * short bucket walk. Measured on this machine, unordered_map wins at every
 * size tested, the sorted vector sits in the middle, and std::map is 10x
 * behind — see the README table and the memory-layout deep dive.
 */

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <map>
#include <random>
#include <unordered_map>
#include <vector>

namespace {

constexpr int kEntries = 100'000;
constexpr int kLookups = 1'000'000;

using Clock = std::chrono::steady_clock;

template <typename Fn>
double time_seconds(Fn&& fn) {
    const auto start = Clock::now();
    fn();
    return std::chrono::duration<double>(Clock::now() - start).count();
}

}  // namespace

int main() {
    // Keys and probes. Half of the probes hit existing keys, half miss.
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> key_dist(0, kEntries * 2);

    std::vector<int> keys(kEntries);
    for (int& k : keys) {
        k = key_dist(rng);
    }
    std::vector<int> probes(kLookups);
    for (int& p : probes) {
        p = key_dist(rng);
    }

    std::map<int, int> tree;
    std::unordered_map<int, int> hash;
    std::vector<std::pair<int, int>> sorted_vec;
    for (int k : keys) {
        tree.emplace(k, k * 3);
        hash.emplace(k, k * 3);
        sorted_vec.emplace_back(k, k * 3);
    }
    std::sort(sorted_vec.begin(), sorted_vec.end());

    long long sum_tree = 0;
    long long sum_hash = 0;
    long long sum_vec = 0;

    const double t_tree = time_seconds([&] {
        for (int p : probes) {
            const auto it = tree.find(p);
            if (it != tree.end()) {
                sum_tree += it->second;
            }
        }
    });

    const double t_hash = time_seconds([&] {
        for (int p : probes) {
            const auto it = hash.find(p);
            if (it != hash.end()) {
                sum_hash += it->second;
            }
        }
    });

    const double t_vec = time_seconds([&] {
        for (int p : probes) {
            const auto it =
                std::lower_bound(sorted_vec.begin(), sorted_vec.end(), std::make_pair(p, 0),
                                 [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                                     return a.first < b.first;
                                 });
            if (it != sorted_vec.end() && it->first == p) {
                sum_vec += it->second;
            }
        }
    });

    std::printf("lookup structures: %d entries, %d probes (50%% hit)\n", kEntries, kLookups);
    std::printf("  std::map (red-black tree)   %8.2f ms  sum=%lld\n", t_tree * 1e3, sum_tree);
    std::printf("  std::unordered_map          %8.2f ms  sum=%lld\n", t_hash * 1e3, sum_hash);
    std::printf("  sorted vector + lower_bound %8.2f ms  sum=%lld\n", t_vec * 1e3, sum_vec);

    if (sum_tree != sum_hash || sum_tree != sum_vec) {
        std::fprintf(stderr, "lookup sums disagree\n");
        return 1;
    }
    std::printf("data_structures: OK (all structures agree)\n");
    return 0;
}
