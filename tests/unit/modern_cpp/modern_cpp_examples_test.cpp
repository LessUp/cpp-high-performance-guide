#include <gtest/gtest.h>

#define HPC_TEST_MODE
#include "../../../examples/03-modern-cpp/src/compile_time.cpp"
#include "../../../examples/03-modern-cpp/src/move_semantics.cpp"
#include "../../../examples/03-modern-cpp/src/ranges_vs_loops.cpp"
#include "../../../examples/03-modern-cpp/src/vector_reserve.cpp"

TEST(CompileTimeExamplesTest, FactorialFunctionsMatch) {
    EXPECT_EQ(hpc::compile_time::factorial_runtime(10), 3628800);
    EXPECT_EQ(hpc::compile_time::factorial_constexpr(10), 3628800);
    constexpr auto value = hpc::compile_time::factorial_consteval(6);
    EXPECT_EQ(value, 720);
}

TEST(CompileTimeExamplesTest, HashAndPrimeUtilitiesWork) {
    using namespace hpc::compile_time;
    EXPECT_EQ(fnv1a_hash("hello"), "hello"_hash);
    EXPECT_TRUE(is_prime(97));
    EXPECT_FALSE(is_prime(91));
    EXPECT_EQ(FIRST_100_PRIMES[0], 2);
    EXPECT_EQ(FIRST_100_PRIMES[24], 97);
}

TEST(MoveSemanticsExamplesTest, MoveConstructorTransfersOwnership) {
    using hpc::move_semantics::Buffer;

    Buffer::reset_counts();
    Buffer source(128);
    ASSERT_NE(source.data(), nullptr);

    Buffer moved(std::move(source));
    EXPECT_EQ(Buffer::copy_count_, 0u);
    EXPECT_EQ(Buffer::move_count_, 1u);
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(moved.size(), 128u);
    EXPECT_NE(moved.data(), nullptr);
}

TEST(MoveSemanticsExamplesTest, ProcessByCopyAndRefCountDifferently) {
    using hpc::move_semantics::Buffer;

    Buffer buffer(64);
    Buffer::reset_counts();
    hpc::move_semantics::process_by_copy(buffer);
    EXPECT_EQ(Buffer::copy_count_, 1u);

    Buffer::reset_counts();
    hpc::move_semantics::process_by_ref(buffer);
    EXPECT_EQ(Buffer::copy_count_, 0u);
}

TEST(VectorReserveExamplesTest, ReserveReducesAllocationCount) {
    using Alloc = hpc::vector_reserve::CountingAllocator<int>;
    using Vec = std::vector<int, Alloc>;

    Alloc::reset_counts();
    Vec without_reserve;
    for (int i = 0; i < 128; ++i) {
        without_reserve.push_back(i);
    }
    const auto allocations_without_reserve = Alloc::allocation_count_;

    Alloc::reset_counts();
    Vec with_reserve;
    with_reserve.reserve(128);
    for (int i = 0; i < 128; ++i) {
        with_reserve.push_back(i);
    }
    const auto allocations_with_reserve = Alloc::allocation_count_;

    EXPECT_GT(allocations_without_reserve, allocations_with_reserve);
    EXPECT_EQ(allocations_with_reserve, 1u);
}

TEST(RangesExamplesTest, TransformAndSumMatchReferenceImplementations) {
    std::vector<int> input(64);
    std::iota(input.begin(), input.end(), 0);

    std::vector<int> raw_output;
    std::vector<int> ranges_output;
    hpc::ranges::transform_raw_loop(input, raw_output);
    hpc::ranges::transform_ranges(input, ranges_output);
    EXPECT_EQ(raw_output, ranges_output);

    EXPECT_EQ(hpc::ranges::sum_raw_loop(input), hpc::ranges::sum_algorithm(input));
    EXPECT_EQ(hpc::ranges::sum_algorithm(input), hpc::ranges::sum_ranges(input));
}

TEST(RangesExamplesTest, ChainedViewsMaterializeToExpectedSequence) {
    std::vector<int> input(32);
    std::iota(input.begin(), input.end(), 0);

    const auto expected = hpc::ranges::chain_raw_loop(input);
    const auto materialized = hpc::ranges::to_vector(hpc::ranges::chain_ranges_view(input));
    EXPECT_EQ(expected, materialized);

    const auto filtered = hpc::ranges::filter_raw_loop(input);
    const auto filtered_view = hpc::ranges::to_vector(hpc::ranges::filter_ranges_view(input));
    EXPECT_EQ(filtered, filtered_view);
}
