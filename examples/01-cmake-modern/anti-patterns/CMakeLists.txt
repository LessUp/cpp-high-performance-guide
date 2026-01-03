# ============================================================================
# ANTI-PATTERNS - DO NOT USE IN PRODUCTION
# ============================================================================
# This file demonstrates CMake anti-patterns that should be avoided.
# Each anti-pattern is commented with the reason why it's bad and
# what to use instead.

cmake_minimum_required(VERSION 3.14)
project(AntiPatterns)

# ============================================================================
# ANTI-PATTERN 1: Using include_directories() instead of target_include_directories()
# ============================================================================
# BAD: This affects ALL targets in the current directory and subdirectories
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#
# WHY IT'S BAD:
# - Pollutes the include path for all targets
# - Makes it hard to understand which target needs which includes
# - Can cause unexpected header conflicts
# - Doesn't propagate properly to dependent targets
#
# GOOD: Use target_include_directories() instead
# target_include_directories(my_target PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ============================================================================
# ANTI-PATTERN 2: Using link_libraries() instead of target_link_libraries()
# ============================================================================
# BAD: This affects ALL targets
# link_libraries(pthread)
#
# WHY IT'S BAD:
# - Same issues as include_directories()
# - Can cause linking errors that are hard to debug
# - Doesn't express dependencies clearly
#
# GOOD: Use target_link_libraries() instead
# target_link_libraries(my_target PRIVATE Threads::Threads)

# ============================================================================
# ANTI-PATTERN 3: Using add_definitions() instead of target_compile_definitions()
# ============================================================================
# BAD: Global definitions
# add_definitions(-DDEBUG_MODE -DUSE_FEATURE_X)
#
# WHY IT'S BAD:
# - Affects all targets
# - Can cause ODR violations
# - Makes it hard to have different configurations per target
#
# GOOD: Use target_compile_definitions() instead
# target_compile_definitions(my_target PRIVATE DEBUG_MODE USE_FEATURE_X)

# ============================================================================
# ANTI-PATTERN 4: Using CMAKE_CXX_FLAGS directly
# ============================================================================
# BAD: Modifying global flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#
# WHY IT'S BAD:
# - Affects all targets
# - Can conflict with generator expressions
# - Doesn't work well with multi-config generators
#
# GOOD: Use target_compile_options() instead
# target_compile_options(my_target PRIVATE -Wall -Wextra)

# ============================================================================
# ANTI-PATTERN 5: Using file(GLOB) for sources
# ============================================================================
# BAD: Globbing source files
# file(GLOB SOURCES "src/*.cpp")
# add_executable(my_target ${SOURCES})
#
# WHY IT'S BAD:
# - CMake won't detect new files until you re-run cmake
# - Can accidentally include unwanted files
# - Makes the build non-reproducible
#
# GOOD: List sources explicitly
# add_executable(my_target
#     src/main.cpp
#     src/utils.cpp
#     src/feature.cpp
# )

# ============================================================================
# ANTI-PATTERN 6: Not using modern find_package() with targets
# ============================================================================
# BAD: Using old-style variables
# find_package(Boost REQUIRED)
# include_directories(${Boost_INCLUDE_DIRS})
# target_link_libraries(my_target ${Boost_LIBRARIES})
#
# WHY IT'S BAD:
# - Doesn't propagate dependencies correctly
# - Requires manual handling of include paths
# - Doesn't work with INTERFACE libraries
#
# GOOD: Use imported targets
# find_package(Boost REQUIRED COMPONENTS filesystem)
# target_link_libraries(my_target PRIVATE Boost::filesystem)

# ============================================================================
# ANTI-PATTERN 7: Using CMAKE_SOURCE_DIR instead of PROJECT_SOURCE_DIR
# ============================================================================
# BAD: Assumes your project is the top-level project
# set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
#
# WHY IT'S BAD:
# - Breaks when your project is used as a subdirectory
# - Makes the project non-composable
#
# GOOD: Use PROJECT_SOURCE_DIR or CMAKE_CURRENT_SOURCE_DIR
# set(MY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# ============================================================================
# ANTI-PATTERN 8: Not specifying PUBLIC/PRIVATE/INTERFACE
# ============================================================================
# BAD: No visibility specifier
# target_link_libraries(my_target some_library)
#
# WHY IT'S BAD:
# - Unclear whether dependency should propagate
# - Can cause unexpected transitive dependencies
#
# GOOD: Always specify visibility
# target_link_libraries(my_target PRIVATE some_library)  # Implementation detail
# target_link_libraries(my_target PUBLIC some_library)   # Part of interface
# target_link_libraries(my_target INTERFACE some_library) # Only for dependents

message(STATUS "")
message(STATUS "=== CMake Anti-Patterns Demo ===")
message(STATUS "This directory contains examples of what NOT to do.")
message(STATUS "See the CMakeLists.txt comments for explanations.")
message(STATUS "")
