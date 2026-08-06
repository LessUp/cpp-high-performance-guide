/**
 * @file allocator_bench.cpp
 * @brief Allocation-strategy benchmark: per-object new/delete vs
 *        hpc::mem::Arena vs std::pmr::monotonic_buffer_resource.
 */

#include <benchmark/benchmark.h>

#include <hpc/arena.hpp>
#include <memory_resource>
#include <vector>

namespace {

struct Node {
    int value;
    int next;
};

void build_and_traverse(Node* nodes, int count) {
    for (int i = 0; i < count; ++i) {
        nodes[i].value = i;
        nodes[i].next = i + 1 < count ? i + 1 : -1;
    }
    long long sum = 0;
    int idx = 0;
    while (idx >= 0) {
        sum += nodes[idx].value;
        idx = nodes[idx].next;
    }
    benchmark::DoNotOptimize(sum);
}

}  // namespace

static void BM_Alloc_NewDelete(benchmark::State& state) {
    const int count = static_cast<int>(state.range(0));
    std::vector<Node*> nodes(static_cast<std::size_t>(count));

    for (auto _ : state) {
        for (int i = 0; i < count; ++i) {
            nodes[static_cast<std::size_t>(i)] = new Node{};
        }
        for (int i = 0; i < count; ++i) {
            delete nodes[static_cast<std::size_t>(i)];
        }
    }
    state.SetItemsProcessed(state.iterations() * count);
}

static void BM_Alloc_Arena(benchmark::State& state) {
    const int count = static_cast<int>(state.range(0));
    hpc::mem::Arena arena(static_cast<std::size_t>(count) * sizeof(Node) + 4096);

    for (auto _ : state) {
        Node* nodes = static_cast<Node*>(
            arena.allocate(static_cast<std::size_t>(count) * sizeof(Node), alignof(Node)));
        build_and_traverse(nodes, count);
        arena.reset();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

static void BM_Alloc_Pmr(benchmark::State& state) {
    const int count = static_cast<int>(state.range(0));

    for (auto _ : state) {
        std::pmr::monotonic_buffer_resource buffer(static_cast<std::size_t>(count) * sizeof(Node) +
                                                   4096);
        std::pmr::polymorphic_allocator<Node> alloc(&buffer);
        Node* nodes = alloc.allocate(count);
        build_and_traverse(nodes, count);
    }
    state.SetItemsProcessed(state.iterations() * count);
}

BENCHMARK(BM_Alloc_NewDelete)->RangeMultiplier(8)->Range(1 << 12, 1 << 18);
BENCHMARK(BM_Alloc_Arena)->RangeMultiplier(8)->Range(1 << 12, 1 << 18);
BENCHMARK(BM_Alloc_Pmr)->RangeMultiplier(8)->Range(1 << 12, 1 << 18);
