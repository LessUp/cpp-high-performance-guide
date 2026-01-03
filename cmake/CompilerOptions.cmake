# CompilerOptions.cmake
# Provides unified compiler options management
# Uses target-based approach (modern CMake best practice)

#------------------------------------------------------------------------------
# Detect compiler and set appropriate flags
#------------------------------------------------------------------------------
set(HPC_IS_GCC FALSE)
set(HPC_IS_CLANG FALSE)
set(HPC_IS_MSVC FALSE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(HPC_IS_GCC TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(HPC_IS_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(HPC_IS_MSVC TRUE)
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
        elseif(HPC_IS_MSVC)
            target_compile_options(${target} PRIVATE
                /W4
                /permissive-
            )
        endif()
    endif()
    
    # Release optimization flags
    if(HPC_IS_GCC OR HPC_IS_CLANG)
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
        
        # Enable vectorization reports
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
    
    if(HPC_IS_GCC OR HPC_IS_CLANG)
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
        # Note: MSVC uses /arch:AVX512 only in newer versions
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
