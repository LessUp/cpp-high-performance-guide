# CompilerOptions.cmake
# Provides unified compiler options management
# Uses target-based approach (modern CMake best practice)

#------------------------------------------------------------------------------
# Detect compiler and architecture
#------------------------------------------------------------------------------
set(HPC_IS_GCC FALSE)
set(HPC_IS_CLANG FALSE)
set(HPC_IS_MSVC FALSE)
set(HPC_IS_ARM FALSE)
set(HPC_IS_APPLE_CLANG FALSE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(HPC_IS_GCC TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(HPC_IS_CLANG TRUE)
    # Detect Apple Clang (has different warning behavior)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(HPC_IS_APPLE_CLANG TRUE)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(HPC_IS_MSVC TRUE)
endif()

# Detect ARM architecture (Apple Silicon, Raspberry Pi, etc.)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM")
    set(HPC_IS_ARM TRUE)
endif()

#------------------------------------------------------------------------------
# hpc_set_compiler_options(target)
# Sets warning levels and optimization flags for a target
#------------------------------------------------------------------------------
function(hpc_set_compiler_options target)
    # Parse arguments
    cmake_parse_arguments(ARG "DISABLE_WARNINGS;ENABLE_FAST_MATH" "" "" ${ARGN})

    # Warning flags
    if(NOT ARG_DISABLE_WARNINGS)
        if(HPC_IS_GCC OR HPC_IS_CLANG)
            target_compile_options(${target} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wshadow
                -Wunused
                -Wnon-virtual-dtor
                -Wold-style-cast
                -Wcast-align
                -Woverloaded-virtual
                -Wformat=2
            )
            # Apple Clang has stricter sign-conversion warnings that trigger
            # in third-party libraries like RapidCheck. Suppress them.
            if(HPC_IS_APPLE_CLANG)
                target_compile_options(${target} PRIVATE
                    -Wno-sign-conversion
                )
            endif()
        elseif(HPC_IS_MSVC)
            target_compile_options(${target} PRIVATE
                /W4
                /permissive-
            )
        endif()
    endif()
    
    # Release optimization flags
    if(HPC_IS_ARM)
        # ARM uses -mcpu=native instead of -march=native
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Release>:-mcpu=native>
            $<$<CONFIG:RelWithDebInfo>:-O2>
            $<$<CONFIG:RelWithDebInfo>:-g>
        )
    elseif(HPC_IS_GCC OR HPC_IS_CLANG)
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Release>:-march=native>
            $<$<CONFIG:Release>:-mtune=native>
            $<$<CONFIG:RelWithDebInfo>:-O2>
            $<$<CONFIG:RelWithDebInfo>:-g>
            $<$<CONFIG:RelWithDebInfo>:-march=native>
        )
        
        # Fast math (use with caution - breaks IEEE compliance)
        if(ARG_ENABLE_FAST_MATH)
            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Release>:-ffast-math>
            )
        endif()
        
        # Enable vectorization reports (opt-in to avoid noisy builds)
        if(HPC_VECTORIZE_REPORT)
            if(HPC_IS_GCC)
                target_compile_options(${target} PRIVATE
                    $<$<CONFIG:Release>:-fopt-info-vec-optimized>
                )
            elseif(HPC_IS_CLANG)
                target_compile_options(${target} PRIVATE
                    $<$<CONFIG:Release>:-Rpass=loop-vectorize>
                    $<$<CONFIG:Release>:-Rpass-missed=loop-vectorize>
                )
            endif()
        endif()
        
    elseif(HPC_IS_MSVC)
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/O2>
            $<$<CONFIG:Release>:/arch:AVX2>
            $<$<CONFIG:RelWithDebInfo>:/O2>
            $<$<CONFIG:RelWithDebInfo>:/Zi>
        )
        
        if(ARG_ENABLE_FAST_MATH)
            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Release>:/fp:fast>
            )
        endif()
    endif()
endfunction()

#------------------------------------------------------------------------------
# hpc_enable_simd(target)
# Enables SIMD instruction sets based on CPU capabilities
#------------------------------------------------------------------------------
function(hpc_enable_simd target)
    cmake_parse_arguments(ARG "SSE;AVX;AVX2;AVX512" "" "" ${ARGN})

    if(HPC_IS_ARM)
        # ARM uses NEON instead of x86 SIMD
        if(HPC_IS_CLANG OR HPC_IS_GCC)
            # NEON is enabled by default on AArch64, but we can add march flag
            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Release>:-mcpu=native>
            )
        endif()
    elseif(HPC_IS_GCC OR HPC_IS_CLANG)
        if(ARG_SSE)
            target_compile_options(${target} PRIVATE -msse4.2)
        endif()
        if(ARG_AVX)
            target_compile_options(${target} PRIVATE -mavx)
        endif()
        if(ARG_AVX2)
            target_compile_options(${target} PRIVATE -mavx2 -mfma)
        endif()
        if(ARG_AVX512)
            target_compile_options(${target} PRIVATE -mavx512f -mavx512dq)
        endif()
    elseif(HPC_IS_MSVC)
        if(ARG_AVX)
            target_compile_options(${target} PRIVATE /arch:AVX)
        endif()
        if(ARG_AVX2 OR ARG_AVX512)
            target_compile_options(${target} PRIVATE /arch:AVX2)
        endif()
    endif()
endfunction()

#------------------------------------------------------------------------------
# hpc_enable_lto(target)
# Enables Link-Time Optimization
#------------------------------------------------------------------------------
function(hpc_enable_lto target)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    
    if(lto_supported)
        set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "LTO enabled for ${target}")
    else()
        message(WARNING "LTO not supported: ${lto_error}")
    endif()
endfunction()
