#include <gtest/gtest.h>

#include "HttpMessage.hpp"


/*
    @brief Test suite for the HttpBuilder class
    @note This test suite is not exhaustive, but it covers the most important cases
*/

/*
    @brief HTTP Response with 200 OK
*/
TEST(HttpBuilderTest, BuildResponse_Code200) {

    const std::string body = "Hello World!";

    HttpResponse res;
    res.SetStatus(200);
    res.body = body;

    // Start line
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusText, "OK");

    // Headers
    EXPECT_EQ(res.headers.size(), 2);
    EXPECT_EQ(res.headers["Content-Type"], "text/html");
    EXPECT_EQ(res.headers["Server"], "knots");

    // Body
    EXPECT_EQ(res.body, body);
}

/*
    @brief HTTP Response with 400 Bad Request
*/
TEST(HttpBuilderTest, BuildResponse_Code400) {

    std::string body = "Bad Request";

    HttpResponse res;
    res.SetStatus(400);
    res.body = body;

    // Start line
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 400);
    EXPECT_EQ(res.statusText, "Bad Request");

    // Headers
    EXPECT_EQ(res.headers.size(), 2);
    EXPECT_EQ(res.headers["Content-Type"], "text/html");
    EXPECT_EQ(res.headers["Server"], "knots");

    // Body
    EXPECT_EQ(res.body, body);
}

/*
    @brief HTTP Response with 404 Not Found
*/
TEST(HttpBuilderTest, BuildResponse_Code404) {

    std::string body = "Not Found";

    HttpResponse res;
    res.SetStatus(404);
    res.body = body;

    // Start line
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 404);
    EXPECT_EQ(res.statusText, "Not Found");

    // Headers
    EXPECT_EQ(res.headers.size(), 2);
    EXPECT_EQ(res.headers["Content-Type"], "text/html");
    EXPECT_EQ(res.headers["Server"], "knots");

    // Body
    EXPECT_EQ(res.body, body);
}
