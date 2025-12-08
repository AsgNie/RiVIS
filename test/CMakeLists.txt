enable_testing()

# rv32i tests
add_executable(test_rv32i)
target_sources(test_rv32i
    PRIVATE
        test_rv32i.cpp
)
target_link_libraries(test_rv32i
    PRIVATE
        GTest::gtest_main
        rv32i
)

# cli tests
add_executable(test_cli)
target_sources(test_cli
    PRIVATE
        test_cli.cpp
)
target_link_libraries(test_cli
    PRIVATE
        GTest::gtest_main
        cli
)

# fileutils tests
add_executable(test_fileutils)
target_sources(test_fileutils
    PRIVATE
        test_fileutils.cpp
        fff.h
)
target_link_libraries(test_fileutils
    PRIVATE
        GTest::gtest_main
        fileutils
)

include(GoogleTest)
gtest_discover_tests(test_rv32i)
gtest_discover_tests(test_cli)
gtest_discover_tests(test_fileutils)

add_subdirectory(systemTest)
