#include <vector>

#include "compile_time.hpp"
#include "ranges_utils.hpp"

int modern_cpp_header_only_helper() {
    std::vector<int> input{1, 2, 3};
    std::vector<int> output;
    hpc::ranges::transform_algorithm(input, output);
    return static_cast<int>(hpc::compile_time::factorial_runtime(3) + hpc::ranges::sum_raw_loop(output));
}
