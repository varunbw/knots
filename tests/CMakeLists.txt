set(TEST_SOURCES
    FileHandlerTest.cpp
    HttpRequestTest.cpp
    HttpResponseTest.cpp
    HttpServerTest.cpp
    RouterTest.cpp
    StaticRoutesTest.cpp
)

add_executable(unit-tests ${TEST_SOURCES})
target_link_libraries(unit-tests
    GTest::gtest_main
    knots
    yaml-cpp
)

include(GoogleTest)
gtest_discover_tests(unit-tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)