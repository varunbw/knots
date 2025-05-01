#include <gtest/gtest.h>
#include "HttpParser.hpp"
#include "HttpMessage.hpp"

TEST(HttpParserTest, ParseValidRequest) {
    std::stringstream ss;
    ss << "GET /test.html HTTP/1.1\r\n"
       << "Host: localhost:8080\r\n"
       << "Connection: keep-alive\r\n\r\n";

    HttpRequest req = HttpParser::ParseHttpRequest(ss);
    
    EXPECT_EQ(req.method, HttpMethod::GET);
    EXPECT_EQ(req.requestUrl, "/test.html");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    EXPECT_TRUE(req.IsValid());
}