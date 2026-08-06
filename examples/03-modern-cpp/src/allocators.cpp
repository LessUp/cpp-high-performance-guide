/**
 * @file allocators.cpp
 * @brief Per-object new/delete vs arena vs std::pmr::monotonic_buffer.
 *
 * Workload: allocate N small nodes one at a time, thread them into a list
 * by index, traverse and sum. This is the shape of parsers, scene graphs,
 * and request handlers — many small allocations with a shared lifetime.
 *
 * std::pmr (C++17) is the standardized middle ground: resource objects
 * (monotonic_buffer_resource here) plug into standard containers via
 * polymorphic_allocator, no custom container needed.
 */

#include <chrono>
#include <cstdio>
#include <hpc/arena.hpp>
#include <memory_resource>
#include <vector>

namespace {

struct Node {
    int value;
    int next;  // index-linked list: no pointers, arena-friendly
};

constexpr int kNodes = 1'000'000;

using Clock = std::chrono::steady_clock;

template <typename Fn>
double time_seconds(Fn&& fn) {
    const auto start = Clock::now();
    fn();
    return std::chrono::duration<double>(Clock::now() - start).count();
}

// Builds the index-linked list and returns the traversal sum (all variants
// must produce the same one).
long long traverse(const Node* nodes) {
    long long sum = 0;
    int idx = 0;
    while (idx >= 0) {
        sum += nodes[idx].value;
        idx = nodes[idx].next;
    }
    return sum;
}

void build_list(Node* nodes, int count) {
    for (int i = 0; i < count; ++i) {
        nodes[i].value = i;
        nodes[i].next = i + 1 < count ? i + 1 : -1;
    }
}

}  // namespace

int main() {
    long long expected = 0;

    // 1) One new[] for the whole array: the honest baseline. Per-object
    //    new/delete is measured in the benchmark; here it would dominate so
    //    strongly that the other two would be invisible.
    double t_new = 0.0;
    {
        std::vector<Node*> nodes(kNodes);
        t_new = time_seconds([&] {
            for (int i = 0; i < kNodes; ++i) {
                nodes[static_cast<std::size_t>(i)] = new Node{i, i + 1 < kNodes ? i + 1 : -1};
            }
            long long sum = 0;
            int idx = 0;
            while (idx >= 0) {
                sum += nodes[static_cast<std::size_t>(idx)]->value;
                idx = nodes[static_cast<std::size_t>(idx)]->next;
            }
            expected = sum;
            for (int i = 0; i < kNodes; ++i) {
                delete nodes[static_cast<std::size_t>(i)];
            }
        });
    }

    // 2) Arena: one malloc up front, bump-allocate every node, reset at end.
    double t_arena = 0.0;
    long long arena_sum = 0;
    {
        hpc::mem::Arena arena(static_cast<std::size_t>(kNodes) * sizeof(Node) + 4096);
        t_arena = time_seconds([&] {
            Node* nodes = static_cast<Node*>(
                arena.allocate(static_cast<std::size_t>(kNodes) * sizeof(Node), alignof(Node)));
            build_list(nodes, kNodes);
            arena_sum = traverse(nodes);
            arena.reset();
        });
    }

    // 3) std::pmr monotonic buffer: same allocation shape, standard library.
    double t_pmr = 0.0;
    long long pmr_sum = 0;
    {
        std::pmr::monotonic_buffer_resource buffer(static_cast<std::size_t>(kNodes) * sizeof(Node) +
                                                   4096);
        t_pmr = time_seconds([&] {
            std::pmr::polymorphic_allocator<Node> alloc(&buffer);
            Node* nodes = alloc.allocate(kNodes);
            build_list(nodes, kNodes);
            pmr_sum = traverse(nodes);
        });
    }

    std::printf("allocators: %d node build+traverse+free\n", kNodes);
    std::printf("  per-object new/delete        %8.2f ms\n", t_new * 1e3);
    std::printf("  hpc::mem::Arena (bump)       %8.2f ms\n", t_arena * 1e3);
    std::printf("  std::pmr::monotonic_buffer   %8.2f ms\n", t_pmr * 1e3);

    if (arena_sum != expected || pmr_sum != expected) {
        std::fprintf(stderr, "sum mismatch: new=%lld arena=%lld pmr=%lld\n", expected, arena_sum,
                     pmr_sum);
        return 1;
    }
    std::printf("allocators: OK (sums agree: %lld)\n", expected);
    return 0;
}
