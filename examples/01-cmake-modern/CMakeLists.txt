# Modern CMake Examples
# This module demonstrates CMake best practices vs anti-patterns

message(STATUS "")
message(STATUS "=== Modern CMake Examples ===")
message(STATUS "")
message(STATUS "This module contains:")
message(STATUS "  - best-practices/ : Modern CMake patterns to follow")
message(STATUS "  - cmake_demo      : Runnable demo built from best-practices/src/")
message(STATUS "")

# Note: best-practices/ is a documentation-only CMakeLists (not add_subdirectory'd)
# to avoid polluting the main build. The demo below is the runnable surface.

# Create a simple demo target
add_executable(cmake_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/best-practices/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/best-practices/src/library.cpp
)

target_include_directories(cmake_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/best-practices/include
)

target_compile_features(cmake_demo PRIVATE cxx_std_20)

target_compile_definitions(cmake_demo PRIVATE
    MY_LIBRARY_VERSION="1.0.0"
)

hpc_set_compiler_options(cmake_demo)
