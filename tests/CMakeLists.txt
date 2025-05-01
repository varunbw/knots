set(TEST_SOURCES
    HttpParserTest.cpp
)

add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests
    GTest::gtest_main
    knots_lib
)

include(GoogleTest)
gtest_discover_tests(unit_tests)