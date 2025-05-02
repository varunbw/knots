#include <gtest/gtest.h>
#include "HttpMessage.hpp"

/*
    Test HttpRequest functionality
    - HttpRequest
        - Default constructor
        - Parameterized constructor
        - IsValid()
*/
TEST(HttpMessageTest, TestHttpRequest) {
    // Test default constructor
    HttpRequest req;
    EXPECT_EQ(req.method, HttpMethod::DEFAULT_INVALID);
    EXPECT_EQ(req.requestUrl, "");
    EXPECT_EQ(req.version, HttpVersion::DEFAULT_INVALID);
    EXPECT_EQ(req.headers.size(), 0);
    EXPECT_EQ(req.body, "");
    EXPECT_FALSE(req.IsValid());

    // Test parameterized constructor
    req = HttpRequest(
        HttpMethod::GET, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {{"Host", "localhost:8686"}}, 
        "body"
    );
    EXPECT_EQ(req.method, HttpMethod::GET);
    EXPECT_EQ(req.requestUrl, "/test");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(req.headers.size(), 1);
    EXPECT_EQ(req.headers["Host"], "localhost:8686");
    EXPECT_EQ(req.body, "body");
    EXPECT_TRUE(req.IsValid());
}