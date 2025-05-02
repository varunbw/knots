set(TEST_SOURCES
    HttpParserTest.cpp
    HttpMessageTest.cpp
    RouterTest.cpp
)

add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests
    GTest::gtest_main
    knots_lib
    yaml-cpp
)

include(GoogleTest)
gtest_discover_tests(unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)