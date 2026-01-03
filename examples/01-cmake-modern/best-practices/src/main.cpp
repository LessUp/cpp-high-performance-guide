#include "library.hpp"
#include <iostream>

int main() {
    std::cout << "Modern CMake Best Practices Demo" << std::endl;
    std::cout << "================================" << std::endl;
    
    myproject::Library lib;
    lib.do_something();
    
    return 0;
}
