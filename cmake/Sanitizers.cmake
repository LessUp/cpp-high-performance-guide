# Sanitizers.cmake
# Provides sanitizer configuration for debugging and testing
# Supports AddressSanitizer, ThreadSanitizer, and UndefinedBehaviorSanitizer

# Options to enable sanitizers
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer (Clang only)" OFF)

# Sanitizers are mutually exclusive (except UBSan can combine with ASan)
if(ENABLE_TSAN AND ENABLE_ASAN)
    message(FATAL_ERROR "ThreadSanitizer and AddressSanitizer cannot be used together")
endif()

if(ENABLE_MSAN AND (ENABLE_ASAN OR ENABLE_TSAN))
    message(FATAL_ERROR "MemorySanitizer cannot be combined with ASan or TSan")
endif()

#------------------------------------------------------------------------------
# hpc_enable_sanitizers(target)
# Enables configured sanitizers for a target
#------------------------------------------------------------------------------
function(hpc_enable_sanitizers target)
    if(NOT (HPC_IS_GCC OR HPC_IS_CLANG))
        message(WARNING "Sanitizers are only supported on GCC and Clang")
        return()
    endif()
    
    set(SANITIZER_FLAGS "")
    set(SANITIZER_LINK_FLAGS "")
    
    # AddressSanitizer
    if(ENABLE_ASAN)
        list(APPEND SANITIZER_FLAGS -fsanitize=address -fno-omit-frame-pointer)
        list(APPEND SANITIZER_LINK_FLAGS -fsanitize=address)
        message(STATUS "AddressSanitizer enabled for ${target}")
    endif()
    
    # ThreadSanitizer
    if(ENABLE_TSAN)
        list(APPEND SANITIZER_FLAGS -fsanitize=thread)
        list(APPEND SANITIZER_LINK_FLAGS -fsanitize=thread)
        message(STATUS "ThreadSanitizer enabled for ${target}")
    endif()
    
    # UndefinedBehaviorSanitizer
    if(ENABLE_UBSAN)
        list(APPEND SANITIZER_FLAGS -fsanitize=undefined)
        list(APPEND SANITIZER_LINK_FLAGS -fsanitize=undefined)
        message(STATUS "UndefinedBehaviorSanitizer enabled for ${target}")
    endif()
    
    # MemorySanitizer (Clang only)
    if(ENABLE_MSAN)
        if(NOT HPC_IS_CLANG)
            message(WARNING "MemorySanitizer is only supported on Clang")
        else()
            list(APPEND SANITIZER_FLAGS -fsanitize=memory -fno-omit-frame-pointer)
            list(APPEND SANITIZER_LINK_FLAGS -fsanitize=memory)
            message(STATUS "MemorySanitizer enabled for ${target}")
        endif()
    endif()
    
    # Apply flags
    if(SANITIZER_FLAGS)
        target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS})
        target_link_options(${target} PRIVATE ${SANITIZER_LINK_FLAGS})
        # Lets tests scale down workload under sanitizers (5-15x runtime overhead),
        # keeping stress tests meaningful without tripping timeouts under TSan.
        target_compile_definitions(${target} PRIVATE HPC_SANITIZER_BUILD)
    endif()
endfunction()
