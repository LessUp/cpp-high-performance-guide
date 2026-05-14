/**
 * @file ranges_utils.hpp
 * @brief C++20 Ranges comparison utilities (teaching module)
 *
 * **Note: This is a teaching example module, not production-ready code.**
 * Each function is intentionally shallow — its interface exposes the
 * implementation strategy (raw_loop / algorithm / ranges). The value
 * is in side-by-side comparison, not in abstraction depth.
 *
 * Key concepts demonstrated:
 * - std::ranges algorithms
 * - Range views (lazy evaluation)
 * - Compiler optimization of ranges
 */

#pragma once

#include <algorithm>
#include <numeric>
#include <ranges>
#include <vector>

namespace hpc::ranges {

//------------------------------------------------------------------------------
// Transform operations
//------------------------------------------------------------------------------

/**
 * @brief Transform using raw loop
 */
void transform_raw_loop(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] * 2 + 1;
    }
}

/**
 * @brief Transform using std::transform
 */
void transform_algorithm(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    std::transform(input.begin(), input.end(), output.begin(), [](int x) { return x * 2 + 1; });
}

/**
 * @brief Transform using ranges
 */
void transform_ranges(const std::vector<int>& input, std::vector<int>& output) {
    output.resize(input.size());
    std::ranges::transform(input, output.begin(), [](int x) { return x * 2 + 1; });
}

//------------------------------------------------------------------------------
// Filter operations
//------------------------------------------------------------------------------

/**
 * @brief Filter using raw loop
 */
std::vector<int> filter_raw_loop(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);  // Estimate
    for (int x : input) {
        if (x % 2 == 0) {
            output.push_back(x);
        }
    }
    return output;
}

/**
 * @brief Filter using std::copy_if
 */
std::vector<int> filter_algorithm(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);
    std::copy_if(input.begin(), input.end(), std::back_inserter(output),
                 [](int x) { return x % 2 == 0; });
    return output;
}

/**
 * @brief Filter using ranges view (lazy)
 */
auto filter_ranges_view(const std::vector<int>& input) {
    return input | std::views::filter([](int x) { return x % 2 == 0; });
}

//------------------------------------------------------------------------------
// Chained operations
//------------------------------------------------------------------------------

/**
 * @brief Filter then transform using raw loops
 */
std::vector<int> chain_raw_loop(const std::vector<int>& input) {
    std::vector<int> output;
    output.reserve(input.size() / 2);
    for (int x : input) {
        if (x % 2 == 0) {
            output.push_back(x * 2 + 1);
        }
    }
    return output;
}

/**
 * @brief Filter then transform using ranges (lazy, single pass)
 */
auto chain_ranges_view(const std::vector<int>& input) {
    return input | std::views::filter([](int x) { return x % 2 == 0; }) |
           std::views::transform([](int x) { return x * 2 + 1; });
}

/**
 * @brief Materialize a range view into a vector
 */
template <std::ranges::range R>
std::vector<std::ranges::range_value_t<R>> to_vector(R&& range) {
    std::vector<std::ranges::range_value_t<R>> result;
    for (auto&& elem : range) {
        result.push_back(std::forward<decltype(elem)>(elem));
    }
    return result;
}

//------------------------------------------------------------------------------
// Sum operations
//------------------------------------------------------------------------------

/**
 * @brief Sum using raw loop
 */
int64_t sum_raw_loop(const std::vector<int>& input) {
    int64_t sum = 0;
    for (int x : input) {
        sum += x;
    }
    return sum;
}

/**
 * @brief Sum using std::accumulate
 */
int64_t sum_algorithm(const std::vector<int>& input) {
    return std::accumulate(input.begin(), input.end(), int64_t{0});
}

/**
 * @brief Sum using ranges
 *
 * C++23 introduces std::ranges::fold_left for this purpose.
 * In C++20 we iterate over a ranges::subrange to stay within the ranges API.
 */
int64_t sum_ranges(const std::vector<int>& input) {
    int64_t sum = 0;
    for (int x : std::ranges::subrange(input)) {
        sum += x;
    }
    return sum;
}

}  // namespace hpc::ranges
