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

# fileutils tests
add_executable(fileutils_test)
target_sources(fileutils_test
    PRIVATE
        fileutils_test.cpp
        fff.h
)
target_link_libraries(fileutils_test
    PRIVATE
        GTest::gtest_main
        fileutils
)

include(GoogleTest)
gtest_discover_tests(rv32i_test)
gtest_discover_tests(cli_test)
gtest_discover_tests(fileutils_test)

add_subdirectory(systemTest)
