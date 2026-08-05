#include <gtest/gtest.h>

#include <cmath>
#include <cstring>
#include <hpc/buffer.hpp>
#include <hpc/compile_time.hpp>
#include <hpc/instrumentation.hpp>
#include <hpc/ranges_utils.hpp>
#include <hpc/vector_reserve.hpp>
#include <limits>

using hpc::instrumentation::OperationMetrics;

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

TEST(CompileTimeExamplesTest, FastSinMatchesStdSinAtTablePoints) {
    using hpc::compile_time::fast_sin;
    constexpr double kPi = 3.14159265358979323846;
    constexpr double kTolerance = 1e-3;

    // These angles land exactly on 1024-entry table points, so the only
    // error is the table's Taylor approximation (~1e-10), far below 1e-3.
    EXPECT_NEAR(fast_sin(0.0), std::sin(0.0), kTolerance);
    EXPECT_NEAR(fast_sin(kPi / 2), std::sin(kPi / 2), kTolerance);
    EXPECT_NEAR(fast_sin(-kPi / 2), std::sin(-kPi / 2), kTolerance);
    EXPECT_NEAR(fast_sin(kPi), std::sin(kPi), kTolerance);
    EXPECT_NEAR(fast_sin(-kPi), std::sin(-kPi), kTolerance);
    EXPECT_NEAR(fast_sin(2 * kPi), std::sin(2 * kPi), kTolerance);
    EXPECT_NEAR(fast_sin(-kPi / 4), std::sin(-kPi / 4), kTolerance);
    EXPECT_NEAR(fast_sin(-3 * kPi / 4), std::sin(-3 * kPi / 4), kTolerance);
}

TEST(CompileTimeExamplesTest, FastSinStaysWithinTableResolutionElsewhere) {
    using hpc::compile_time::fast_sin;
    constexpr double kPi = 3.14159265358979323846;
    // The table has 1024 entries and fast_sin truncates to the nearest lower
    // entry (no interpolation), so for any finite angle
    // |fast_sin(a) - sin(a)| <= 2*PI/1024. A 1e-3 tolerance would be too
    // tight here: e.g. fast_sin(1e6) differs from std::sin(1e6) by ~4e-3.
    constexpr double kMaxError = 2 * kPi / 1024;

    EXPECT_NEAR(fast_sin(-0.7), std::sin(-0.7), kMaxError);
    EXPECT_NEAR(fast_sin(1e6), std::sin(1e6), kMaxError);
    EXPECT_NEAR(fast_sin(-1e6), std::sin(-1e6), kMaxError);
}

TEST(CompileTimeExamplesTest, FastSinNonFiniteInputReturnsNaN) {
    using hpc::compile_time::fast_sin;
    EXPECT_TRUE(std::isnan(fast_sin(std::numeric_limits<double>::quiet_NaN())));
    EXPECT_TRUE(std::isnan(fast_sin(std::numeric_limits<double>::infinity())));
    EXPECT_TRUE(std::isnan(fast_sin(-std::numeric_limits<double>::infinity())));
}

TEST(MoveSemanticsExamplesTest, MoveConstructorTransfersOwnership) {
    using hpc::move_semantics::Buffer;

    OperationMetrics metrics;
    OperationMetrics::Scope scope(metrics);

    Buffer source(128, &metrics);
    ASSERT_NE(source.data(), nullptr);

    Buffer moved(std::move(source));
    EXPECT_EQ(metrics.copy_count, 0u);
    EXPECT_EQ(metrics.move_count, 1u);
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(moved.size(), 128u);
    EXPECT_NE(moved.data(), nullptr);
}

TEST(MoveSemanticsExamplesTest, CopyAssignmentDuplicatesContentAndLeavesSourceIntact) {
    using hpc::move_semantics::Buffer;

    OperationMetrics metrics;
    Buffer source(64, &metrics);
    std::memset(source.data(), 0x5A, source.size());

    Buffer target(8, &metrics);
    OperationMetrics::Scope scope(metrics);
    target = source;

    // Copy-and-swap records the temporary's copy construction plus the
    // assignment notification, so copy_count grows (by two today) while no
    // move is recorded.
    EXPECT_GT(metrics.copy_count, 0u);
    EXPECT_EQ(metrics.move_count, 0u);
    ASSERT_EQ(target.size(), source.size());
    EXPECT_EQ(std::memcmp(target.data(), source.data(), source.size()), 0);

    // The source keeps its own storage and content.
    ASSERT_NE(source.data(), nullptr);
    EXPECT_EQ(source.size(), 64u);
    EXPECT_EQ(static_cast<unsigned char>(source.data()[0]), 0x5Au);
    EXPECT_EQ(static_cast<unsigned char>(source.data()[63]), 0x5Au);
}

TEST(MoveSemanticsExamplesTest, MoveAssignmentTransfersOwnershipAndRecordsMove) {
    using hpc::move_semantics::Buffer;

    OperationMetrics metrics;
    Buffer source(96, &metrics);
    Buffer target(16, &metrics);

    OperationMetrics::Scope scope(metrics);
    char* source_data = source.data();
    target = std::move(source);

    EXPECT_EQ(metrics.copy_count, 0u);
    EXPECT_EQ(metrics.move_count, 1u);
    EXPECT_EQ(target.size(), 96u);
    EXPECT_EQ(target.data(), source_data);  // pointer transfer, no copy
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(source.size(), 0u);
}

TEST(MoveSemanticsExamplesTest, SelfAssignmentIsSafeNoOp) {
    using hpc::move_semantics::Buffer;

    OperationMetrics metrics;
    Buffer buffer(48, &metrics);
    std::memset(buffer.data(), 0x7C, buffer.size());
    char* original_data = buffer.data();

    OperationMetrics::Scope scope(metrics);
    // Assign through a reference: same self-assignment check, without
    // tripping Clang's -Wself-assign-overloaded on direct `x = x`.
    Buffer& self = buffer;
    buffer = self;  // exercises the self-assignment guard

    EXPECT_EQ(metrics.copy_count, 0u);
    EXPECT_EQ(metrics.move_count, 0u);
    EXPECT_EQ(buffer.size(), 48u);
    EXPECT_EQ(buffer.data(), original_data);
    for (size_t i = 0; i < buffer.size(); ++i) {
        ASSERT_EQ(static_cast<unsigned char>(buffer.data()[i]), 0x7Cu);
    }
}

TEST(MoveSemanticsExamplesTest, ProcessByCopyAndRefCountDifferently) {
    using hpc::move_semantics::Buffer;

    OperationMetrics metrics;
    Buffer buffer(64, &metrics);

    OperationMetrics::Scope scope(metrics);
    hpc::move_semantics::process_by_copy(buffer);
    EXPECT_EQ(metrics.copy_count, 1u);

    // A new scope auto-resets for the reference test
    OperationMetrics::Scope scope2(metrics);
    hpc::move_semantics::process_by_ref(buffer);
    EXPECT_EQ(metrics.copy_count, 0u);
}

TEST(MoveSemanticsExamplesTest, EmptyBuffersAreSafeToProcess) {
    using hpc::move_semantics::Buffer;

    Buffer empty_default;
    Buffer empty_sized(0);

    hpc::move_semantics::process_by_ref(empty_default);
    hpc::move_semantics::process_by_copy(empty_default);
    hpc::move_semantics::process_by_ref(empty_sized);
    hpc::move_semantics::process_by_copy(empty_sized);
}

TEST(VectorReserveExamplesTest, ReserveReducesAllocationCount) {
    using Alloc = hpc::vector_reserve::CountingAllocator<int>;

    OperationMetrics metrics;
    OperationMetrics::Scope scope(metrics);
    Alloc alloc(&metrics);
    std::vector<int, Alloc> without_reserve(alloc);
    for (int i = 0; i < 128; ++i) {
        without_reserve.push_back(i);
    }
    const auto allocations_without_reserve = metrics.allocation_count;

    OperationMetrics::Scope scope2(metrics);
    std::vector<int, Alloc> with_reserve(alloc);
    with_reserve.reserve(128);
    for (int i = 0; i < 128; ++i) {
        with_reserve.push_back(i);
    }
    const auto allocations_with_reserve = metrics.allocation_count;

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
