# Dependencies.cmake
# Manages external dependencies using FetchContent
# This is the modern CMake approach - no need for git submodules or manual downloads

include(FetchContent)

# Set FetchContent to be quiet by default
set(FETCHCONTENT_QUIET ON)

#------------------------------------------------------------------------------
# Google Benchmark
#------------------------------------------------------------------------------
function(hpc_fetch_google_benchmark)
    message(STATUS "Fetching Google Benchmark...")
    
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.8.3
        GIT_SHALLOW    TRUE
    )
    
    # Disable benchmark tests to speed up build
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(benchmark)
    
    message(STATUS "Google Benchmark fetched successfully")
endfunction()

#------------------------------------------------------------------------------
# Google Test
#------------------------------------------------------------------------------
function(hpc_fetch_google_test)
    message(STATUS "Fetching Google Test...")
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    message(STATUS "Google Test fetched successfully")
endfunction()

#------------------------------------------------------------------------------
# RapidCheck (Property-based testing)
#------------------------------------------------------------------------------
function(hpc_fetch_rapidcheck)
    message(STATUS "Fetching RapidCheck...")
    
    FetchContent_Declare(
        rapidcheck
        GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    
    # Enable GTest integration
    set(RC_ENABLE_GTEST ON CACHE BOOL "" FORCE)
    set(RC_ENABLE_GMOCK ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(rapidcheck)
    
    message(STATUS "RapidCheck fetched successfully")
endfunction()
