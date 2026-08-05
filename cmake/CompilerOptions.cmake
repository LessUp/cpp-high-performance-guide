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
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(HPC_IS_APPLE_CLANG TRUE)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(HPC_IS_MSVC TRUE)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM")
    set(HPC_IS_ARM TRUE)
endif()

#------------------------------------------------------------------------------
# Helper: Add compile options for specific configurations
#------------------------------------------------------------------------------
function(_hpc_add_config_options target)
    cmake_parse_arguments(ARG "" "RELEASE;RELWITHDEBINFO;DEBUG" "" ${ARGN})
    
    if(ARG_RELEASE)
        target_compile_options(${target} PRIVATE "$<$<CONFIG:Release>:${ARG_RELEASE}>")
    endif()
    if(ARG_RELWITHDEBINFO)
        target_compile_options(${target} PRIVATE "$<$<CONFIG:RelWithDebInfo>:${ARG_RELWITHDEBINFO}>")
    endif()
    if(ARG_DEBUG)
        target_compile_options(${target} PRIVATE "$<$<CONFIG:Debug>:${ARG_DEBUG}>")
    endif()
endfunction()

#------------------------------------------------------------------------------
# hpc_set_compiler_options(target)
# Sets warning levels and optimization flags for a target
#------------------------------------------------------------------------------
function(hpc_set_compiler_options target)
    cmake_parse_arguments(ARG "DISABLE_WARNINGS;ENABLE_FAST_MATH" "" "" ${ARGN})

    if(NOT ARG_DISABLE_WARNINGS)
        if(HPC_IS_GCC OR HPC_IS_CLANG)
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wunused
                -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
                -Woverloaded-virtual -Wformat=2
            )
            if(HPC_IS_APPLE_CLANG)
                target_compile_options(${target} PRIVATE -Wno-sign-conversion)
            endif()
        elseif(HPC_IS_MSVC)
            target_compile_options(${target} PRIVATE /W4 /permissive-)
        endif()
    endif()
    
    if(HPC_IS_ARM)
        _hpc_add_config_options(${target}
            RELEASE "-O3;-mcpu=native"
            RELWITHDEBINFO "-O2;-g"
        )
    elseif(HPC_IS_GCC OR HPC_IS_CLANG)
        _hpc_add_config_options(${target}
            RELEASE "-O3;-march=native;-mtune=native"
            RELWITHDEBINFO "-O2;-g;-march=native"
        )
        
        if(ARG_ENABLE_FAST_MATH)
            _hpc_add_config_options(${target} RELEASE "-ffast-math")
        endif()
        
        if(HPC_VECTORIZE_REPORT)
            if(HPC_IS_GCC)
                _hpc_add_config_options(${target} RELEASE "-fopt-info-vec-optimized")
            elseif(HPC_IS_CLANG)
                target_compile_options(${target} PRIVATE
                    $<$<CONFIG:Release>:-Rpass=loop-vectorize>
                    $<$<CONFIG:Release>:-Rpass-missed=loop-vectorize>
                )
            endif()
        endif()
        
    elseif(HPC_IS_MSVC)
        _hpc_add_config_options(${target}
            RELEASE "/O2;/arch:AVX2"
            RELWITHDEBINFO "/O2;/Zi"
        )
        
        if(ARG_ENABLE_FAST_MATH)
            _hpc_add_config_options(${target} RELEASE "/fp:fast")
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
        if(HPC_IS_CLANG OR HPC_IS_GCC)
            _hpc_add_config_options(${target} RELEASE "-mcpu=native")
        endif()
    elseif(HPC_IS_GCC OR HPC_IS_CLANG)
        set(simd_flags "")
        if(ARG_SSE)
            list(APPEND simd_flags -msse4.2)
        endif()
        if(ARG_AVX)
            list(APPEND simd_flags -mavx)
        endif()
        if(ARG_AVX2)
            list(APPEND simd_flags -mavx2 -mfma)
        endif()
        if(ARG_AVX512)
            list(APPEND simd_flags -mavx512f -mavx512dq)
        endif()
        if(simd_flags)
            target_compile_options(${target} PRIVATE ${simd_flags})
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
