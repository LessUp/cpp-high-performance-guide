# I/O utilities unit tests (Linux only, mirrors the module's platform gate)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(io_utils_test
        io_utils_test.cpp
    )

    target_link_libraries(io_utils_test PRIVATE
        GTest::gtest
        GTest::gtest_main
        hpc_headers
    )

    hpc_set_compiler_options(io_utils_test)
    hpc_enable_sanitizers(io_utils_test)

    gtest_add_tests(TARGET io_utils_test)
endif()
