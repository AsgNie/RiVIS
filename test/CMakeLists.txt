enable_testing()

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

include(GoogleTest)
gtest_discover_tests(rv32i_test)
