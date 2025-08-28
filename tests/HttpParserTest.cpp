#include <gtest/gtest.h>
#include "MessageHandler.hpp"
#include "HttpMessage.hpp"

/*
    Test HttpParser functionality
    - ParseHttpRequest
        - ParseStartLine
        - ParseHeaders
        - ParseBody
*/

/*
    @brief Parse a valid HTTP request
*/
TEST(HttpParserTest, ParseValidRequest) {
    std::stringstream ss;
    ss << "GET /test.html HTTP/1.1\r\n"
       << "Host: localhost:8080\r\n"
       << "Connection: keep-alive\r\n\r\n"
       << "This is the body of the request";

    HttpRequest req = MessageHandler::ParseHttpRequest(ss);
    
    // Start line
    EXPECT_EQ(req.method, HttpMethod::GET);
    EXPECT_EQ(req.requestUrl, "/test.html");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    
    // Headers
    EXPECT_EQ(req.headers.size(), 2);
    EXPECT_EQ(req.headers["Host"], "localhost:8080");
    EXPECT_EQ(req.headers["Connection"], "keep-alive");
    
    // Body
    EXPECT_EQ(req.body, "");
}

/*
    @brief Parse an invalid HTTP request
*/
TEST(HttpParserTest, ParseInvalidRequest) {
    std::stringstream ss;
    ss << "INVALID REQUEST LINE\r\n"
       << "Host: localhost:8080\r\n"
       << "Connection: keep-alive\r\n\r\n"
       << "This is the body of the request";

    HttpRequest req = MessageHandler::ParseHttpRequest(ss);
    
    // Start line
    EXPECT_EQ(req.method, HttpMethod::DEFAULT_INVALID);
    EXPECT_EQ(req.requestUrl, "");
    EXPECT_EQ(req.version, HttpVersion::DEFAULT_INVALID);
    
    // Headers
    EXPECT_EQ(req.headers.size(), 0);
    
    // Body
    EXPECT_EQ(req.body, "");
}