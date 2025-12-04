enable_testing()

# rv32i tests
add_executable(rv32i_test)
target_sources(rv32i_test
    PRIVATE
        rv32i_test.cpp
)
target_link_libraries(rv32i_test
    PRIVATE
        GTest::gtest_main
        rv32i
)

# cli tests
add_executable(cli_test)
target_sources(cli_test
    PRIVATE
        cli_test.cpp
)
target_link_libraries(cli_test
    PRIVATE
        GTest::gtest_main
        cli
)


include(GoogleTest)
gtest_discover_tests(rv32i_test)
gtest_discover_tests(cli_test)

add_subdirectory(systemTest)
