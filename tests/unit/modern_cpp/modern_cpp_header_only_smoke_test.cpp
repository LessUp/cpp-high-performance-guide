#include <vector>

#include "compile_time.hpp"
#include "ranges_utils.hpp"

int modern_cpp_header_only_helper();

int main() {
    std::vector<int> input{0, 1, 2, 3, 4};
    std::vector<int> output;
    hpc::ranges::transform_raw_loop(input, output);

    if (hpc::compile_time::factorial_runtime(5) != 120) {
        return 1;
    }

    if (output != std::vector<int>({1, 3, 5, 7, 9})) {
        return 2;
    }

    return modern_cpp_header_only_helper() == 21 ? 0 : 3;
}
