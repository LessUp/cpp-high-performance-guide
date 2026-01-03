#include "library.hpp"
#include <iostream>

namespace myproject {

Library::Library() = default;
Library::~Library() = default;

std::string Library::version() {
#ifdef MY_LIBRARY_VERSION
    return MY_LIBRARY_VERSION;
#else
    return "unknown";
#endif
}

void Library::do_something() {
    std::cout << "Library version: " << version() << std::endl;
}

} // namespace myproject
