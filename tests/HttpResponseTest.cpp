#include <gtest/gtest.h>

#include "HttpMessage.hpp"


/*
    @brief Test suite for the HttpResponse class
    @note This test suite is not exhaustive, but it covers the most important cases
*/

/*
    @brief HTTP Response with 200 OK
*/
TEST(HttpResponseTest, ResponseCode_200) {

    const std::string body = "Hello World!";

    HttpResponse res;
    res.SetStatus(200);
    res.SetBody(body);

    // Start line
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusText, "OK");

    // Headers
    EXPECT_EQ(res.headers.size(), 1);

    // Body
    EXPECT_EQ(res.body, body);
}

/*
    @brief HTTP Response with 400 Bad Request
*/
TEST(HttpResponseTest, ResponseCode_400) {

    const std::string body = "A bad request message";

    HttpResponse res;
    res.SetStatus(400);
    res.SetBody(body);

    // Start line
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 400);
    EXPECT_EQ(res.statusText, "Bad Request");

    // Headers
    EXPECT_EQ(res.headers.size(), 1);

    // Body
    EXPECT_EQ(res.body, body);
}

/*
    @brief HTTP Response with 404 Not Found
*/
TEST(HttpResponseTest, ResponseCode_404) {

    const std::string body = "A not found message";

    HttpResponse res;
    res.SetStatus(404);
    res.SetBody(body);

    // Start line
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 404);
    EXPECT_EQ(res.statusText, "Not Found");

    // Headers
    EXPECT_EQ(res.headers.size(), 1);

    // Body
    EXPECT_EQ(res.body, body);
}
