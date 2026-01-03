# ============================================================================
# BEST PRACTICES - Modern CMake (3.14+)
# ============================================================================
# This file demonstrates modern CMake best practices.
# Use these patterns in your projects!

cmake_minimum_required(VERSION 3.14)
project(BestPractices VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# BEST PRACTICE 1: Set C++ standard using target properties
# ============================================================================
# Instead of: set(CMAKE_CXX_STANDARD 20)
# Use target properties for better control

add_library(my_library
    src/library.cpp
)

# Set C++ standard per-target
target_compile_features(my_library PUBLIC cxx_std_20)

# ============================================================================
# BEST PRACTICE 2: Use target_include_directories with proper visibility
# ============================================================================
target_include_directories(my_library
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================================
# BEST PRACTICE 3: Use target_compile_definitions with visibility
# ============================================================================
target_compile_definitions(my_library
    PUBLIC
        MY_LIBRARY_VERSION="${PROJECT_VERSION}"
    PRIVATE
        MY_LIBRARY_INTERNAL
)

# ============================================================================
# BEST PRACTICE 4: Use target_compile_options with generator expressions
# ============================================================================
target_compile_options(my_library
    PRIVATE
        # Warnings for GCC/Clang
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        # Warnings for MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        # Debug-only options
        $<$<CONFIG:Debug>:-g -O0>
        # Release-only options
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# ============================================================================
# BEST PRACTICE 5: Use imported targets from find_package
# ============================================================================
find_package(Threads REQUIRED)
target_link_libraries(my_library PRIVATE Threads::Threads)

# ============================================================================
# BEST PRACTICE 6: Create INTERFACE libraries for header-only code
# ============================================================================
add_library(header_only_lib INTERFACE)
target_include_directories(header_only_lib
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(header_only_lib INTERFACE cxx_std_20)

# ============================================================================
# BEST PRACTICE 7: Use FetchContent for dependencies
# ============================================================================
include(FetchContent)

# Example: Fetch a dependency
# FetchContent_Declare(
#     json
#     GIT_REPOSITORY https://github.com/nlohmann/json.git
#     GIT_TAG v3.11.2
# )
# FetchContent_MakeAvailable(json)
# target_link_libraries(my_library PRIVATE nlohmann_json::nlohmann_json)

# ============================================================================
# BEST PRACTICE 8: Use ALIAS targets
# ============================================================================
add_library(MyProject::Library ALIAS my_library)
add_library(MyProject::HeaderOnly ALIAS header_only_lib)

# ============================================================================
# BEST PRACTICE 9: Create executables with explicit sources
# ============================================================================
add_executable(my_app
    src/main.cpp
)

target_link_libraries(my_app
    PRIVATE
        MyProject::Library
        MyProject::HeaderOnly
)

# ============================================================================
# BEST PRACTICE 10: Use CMake presets for configuration
# ============================================================================
# See CMakePresets.json in the project root for examples

# ============================================================================
# BEST PRACTICE 11: Proper install rules
# ============================================================================
include(GNUInstallDirs)

install(TARGETS my_library header_only_lib
    EXPORT MyProjectTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ============================================================================
# BEST PRACTICE 12: Export targets for find_package
# ============================================================================
install(EXPORT MyProjectTargets
    FILE MyProjectTargets.cmake
    NAMESPACE MyProject::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyProject
)

message(STATUS "")
message(STATUS "=== CMake Best Practices Demo ===")
message(STATUS "This directory demonstrates modern CMake patterns.")
message(STATUS "Use these patterns in your projects!")
message(STATUS "")
