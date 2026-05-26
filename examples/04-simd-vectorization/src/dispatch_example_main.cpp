#include <hpc/simd.hpp>
#include <iostream>

int main() {
    constexpr size_t kSize = 8;
    hpc::simd::aligned_vector<float> a(kSize), b(kSize), c(kSize);

    for (size_t i = 0; i < kSize; ++i) {
        a[i] = static_cast<float>(i);
        b[i] = static_cast<float>(kSize - i);
    }

    hpc::simd::dispatch_add_arrays(a.data(), b.data(), c.data(), kSize);

    std::cout << "Runtime-dispatched array addition:" << std::endl;
    for (size_t i = 0; i < kSize; ++i) {
        std::cout << "  c[" << i << "] = " << a[i] << " + " << b[i] << " = " << c[i] << std::endl;
    }

    return 0;
}
