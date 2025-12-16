#include <gtest/gtest.h>
#include "HttpMessage.hpp"

/*
    @brief Check default constructor of HttpRequest
*/
TEST(HttpRequestTest, DefaultConstructor) {
    
    HttpRequest req;

    EXPECT_EQ(req.method, HttpMethod::DEFAULT_INVALID);
    EXPECT_EQ(req.requestUrl, "");
    EXPECT_EQ(req.version, HttpVersion::DEFAULT_INVALID);
    
    EXPECT_EQ(req.headers.size(), 0);
    
    EXPECT_EQ(req.body, "");
}

/*
    @brief Check parameterized constructor of HttpRequest
*/
TEST(HttpRequestTest, ParameterizedConstructor) {
    
    HttpRequest req(
        HttpMethod::POST, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {{"Host", "localhost:8600"}}, 
        "body",
        {},
        {}
    );
    
    EXPECT_EQ(req.method, HttpMethod::POST);
    EXPECT_EQ(req.requestUrl, "/test");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    
    EXPECT_EQ(req.headers.size(), 1);
    EXPECT_EQ(req.GetHeader("Host"), "localhost:8600");
    
    EXPECT_EQ(req.body, "body");
}

/*
    @brief Check that the HttpRequest object can handle case insensitive headers
*/
TEST(HttpRequestTest, CaseInsensitiveHeader) {
    
    HttpRequest req(
        HttpMethod::GET, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {
            {"Host", "localhost:8600"},
            {"Connection", "keep-alive"}
        }, 
        "body",
        {},
        {}
    );

    EXPECT_EQ(req.GetHeader("Host"), "localhost:8600");
    EXPECT_EQ(req.GetHeader("host"), "localhost:8600");
    EXPECT_EQ(req.GetHeader("HOST"), "localhost:8600");
    EXPECT_EQ(req.GetHeader("hOsT"), "localhost:8600");
    
    EXPECT_EQ(req.GetHeader("Connection"), "keep-alive");
    EXPECT_EQ(req.GetHeader("connection"), "keep-alive");
    EXPECT_EQ(req.GetHeader("CONNECTION"), "keep-alive");
    EXPECT_EQ(req.GetHeader("cOnNeCtIoN"), "keep-alive");

    EXPECT_EQ(req.headers.size(), 2);
}

/*
    @brief Check that all forms of query parameter parsing is working properly

    Checks for
    - No queryParams
    - Valid single param (param=value)
    - Valid multiple queryParams (param1=value1&param2=value2)
    - Empty param value (param=)
    - Empty param value without '=' (param)
    - Trailing '?'
    - Trailing '&'
    - Mixed
*/
TEST(HttpRequestTest, QueryParameterParsing) {

    // Test simple URL without parameters
    HttpRequest req1;
    req1.requestUrl = "/test";
    EXPECT_EQ(req1.requestUrl, "/test");
    EXPECT_EQ(req1.queryParams.size(), 0);

    // Test URL with single parameter
    HttpRequest req2;
    std::stringstream ss2("GET /page?param=value HTTP/1.1\r\n\r\n");
    req2.ParseFrom(ss2);
    EXPECT_EQ(req2.requestUrl, "/page");
    EXPECT_EQ(req2.queryParams.size(), 1);
    EXPECT_EQ(req2.GetQueryParam("param"), "value");

    // Test URL with multiple parameters
    HttpRequest req3;
    std::stringstream ss3("GET /index.html?a=1&b=2&c=3 HTTP/1.1\r\n\r\n");
    req3.ParseFrom(ss3);
    EXPECT_EQ(req3.requestUrl, "/index.html");
    EXPECT_EQ(req3.queryParams.size(), 3);
    EXPECT_EQ(req3.GetQueryParam("a"), "1");
    EXPECT_EQ(req3.GetQueryParam("b"), "2");
    EXPECT_EQ(req3.GetQueryParam("c"), "3");

    // Test URL with empty parameter value
    HttpRequest req4;
    std::stringstream ss4("GET /search?q= HTTP/1.1\r\n\r\n");
    req4.ParseFrom(ss4);
    EXPECT_EQ(req4.requestUrl, "/search");
    EXPECT_EQ(req4.queryParams.size(), 1);
    EXPECT_EQ(req4.GetQueryParam("q"), "");

    // Test URL with parameter without value
    HttpRequest req5;
    std::stringstream ss5("GET /toggle?dark HTTP/1.1\r\n\r\n");
    req5.ParseFrom(ss5);
    EXPECT_EQ(req5.requestUrl, "/toggle");
    EXPECT_EQ(req5.queryParams.size(), 1);
    EXPECT_EQ(req5.GetQueryParam("dark"), "");

    // Test URL with trailing question mark
    HttpRequest req6;
    std::stringstream ss6("GET /page? HTTP/1.1\r\n\r\n");
    req6.ParseFrom(ss6);
    EXPECT_EQ(req6.requestUrl, "/page");
    EXPECT_EQ(req6.queryParams.size(), 0);

    // Test URL with trailing ampersand
    HttpRequest req7;
    std::stringstream ss7("GET /page?a=1& HTTP/1.1\r\n\r\n");
    req7.ParseFrom(ss7);
    EXPECT_EQ(req7.requestUrl, "/page");
    EXPECT_EQ(req7.queryParams.size(), 1);
    EXPECT_EQ(req7.GetQueryParam("a"), "1");

    // Test URL with mixed parameter types
    HttpRequest req8;
    std::stringstream ss8("GET /complex?a=1&b=&c&d=4 HTTP/1.1\r\n\r\n");
    req8.ParseFrom(ss8);
    EXPECT_EQ(req8.requestUrl, "/complex");
    EXPECT_EQ(req8.queryParams.size(), 4);
    EXPECT_EQ(req8.GetQueryParam("a"), "1");
    EXPECT_EQ(req8.GetQueryParam("b"), "");
    EXPECT_EQ(req8.GetQueryParam("c"), "");
    EXPECT_EQ(req8.GetQueryParam("d"), "4");
}

/*
    @brief Parse a valid HTTP request
*/
TEST(HttpRequestTest, ParseValidRequest) {
    std::stringstream ss;
    ss << "GET /test.html HTTP/1.1\r\n"
       << "Host: localhost:8080\r\n"
       << "Connection: keep-alive\r\n\r\n"
       << "This is the body of the request";

    HttpRequest req;
    req.ParseFrom(ss);
    
    // Start line
    EXPECT_EQ(req.method, HttpMethod::GET);
    EXPECT_EQ(req.requestUrl, "/test.html");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    
    // Headers
    EXPECT_EQ(req.headers.size(), 2);
    EXPECT_EQ(req.GetHeader("Host"), "localhost:8080");
    EXPECT_EQ(req.GetHeader("Connection"), "keep-alive");
    
    // Body
    EXPECT_EQ(req.body, "");
}

/*
    @brief Parse an invalid HTTP request
*/
TEST(HttpRequestTest, ParseInvalidRequest) {
    std::stringstream ss;
    ss << "INVALID REQUEST LINE\r\n"
       << "Host: localhost:8080\r\n"
       << "Connection: keep-alive\r\n\r\n"
       << "This is the body of the request";

    HttpRequest req;
    req.ParseFrom(ss);
    
    // Start line
    EXPECT_EQ(req.method, HttpMethod::DEFAULT_INVALID);
    EXPECT_EQ(req.requestUrl, "");
    EXPECT_EQ(req.version, HttpVersion::DEFAULT_INVALID);
    
    // Headers
    EXPECT_EQ(req.headers.size(), 0);
    
    // Body
    EXPECT_EQ(req.body, "");
}

TEST(HttpRequestTest, GetHeaderAPI) {

    HttpRequest req(
        HttpMethod::GET, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {
            {"Host", "localhost:8600"},
            {"Connection", "keep-alive"}
        }, 
        "body",
        {},
        {}
    );

    EXPECT_EQ(req.GetHeader("Host"), "localhost:8600");
    EXPECT_EQ(req.GetHeader("Connection"), "keep-alive");
    EXPECT_EQ(req.GetHeader("Not-Present-Header"), std::nullopt);
}

TEST(HttpRequestTest, GetQueryParamAPI) {

    HttpRequest req(
        HttpMethod::GET, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {
            {"Host", "localhost:8686"},
            {"Connection", "keep-alive"}
        }, 
        "body",
        {
            {"key1", "val1"},
            {"key2", "val2"},
            {"key3", "val3"}
        },
        {}
    );

    EXPECT_EQ(req.GetQueryParam("key1"), "val1");
    EXPECT_EQ(req.GetQueryParam("key2"), "val2");
    EXPECT_EQ(req.GetQueryParam("key3"), "val3");
    EXPECT_EQ(req.GetQueryParam("not-present-param"), std::nullopt);
}

TEST(HttpRequestTest, GetRouteParamAPI) {

    HttpRequest req(
        HttpMethod::GET, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {
            {"Host", "localhost:8686"},
            {"Connection", "keep-alive"}
        }, 
        "body",
        {},
        {
            {"userId", "123"},
            {"orderId", "abc456"}
        }
    );

    EXPECT_EQ(req.GetRouteParam("userId"), "123");
    EXPECT_EQ(req.GetRouteParam("orderId"), "abc456");
    EXPECT_EQ(req.GetRouteParam("not-present-param"), std::nullopt);
}