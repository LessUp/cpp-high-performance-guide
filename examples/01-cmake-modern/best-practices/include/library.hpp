#pragma once

#include <string>

namespace myproject {

/**
 * @brief Example library class demonstrating modern CMake usage
 */
class Library {
public:
    Library();
    ~Library();
    
    /**
     * @brief Get the library version
     */
    static std::string version();
    
    /**
     * @brief Example function
     */
    void do_something();
};

} // namespace myproject
