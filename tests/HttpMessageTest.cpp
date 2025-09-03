#include <gtest/gtest.h>
#include "HttpMessage.hpp"

// -- Test HttpRequest

/*
    @brief Check default constructor of HttpRequest
*/
TEST(HttpMessageTest, HttpRequest_DefaultConstructor) {
    
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
TEST(HttpMessageTest, HttpRequest_ParameterizedConstructor) {
    
    HttpRequest req(
        HttpMethod::POST, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {{"Host", "localhost:8686"}}, 
        "body",
        {{}}
    );
    
    EXPECT_EQ(req.method, HttpMethod::POST);
    EXPECT_EQ(req.requestUrl, "/test");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    
    EXPECT_EQ(req.headers.size(), 1);
    EXPECT_EQ(req.headers["Host"], "localhost:8686");
    
    EXPECT_EQ(req.body, "body");
}

/*
    @brief Check that the HttpRequest object can handle case insensitive headers
*/
TEST(HttpMessageTest, HttpRequest_CaseInsensitiveHeader) {
    
    HttpRequest req(
        HttpMethod::GET, 
        "/test", 
        HttpVersion::HTTP_1_1, 
        {
            {"Host", "localhost:8686"},
            {"Connection", "keep-alive"}
        }, 
        "body",
        {{}}
    );

    EXPECT_EQ(req.headers["Host"], "localhost:8686");
    EXPECT_EQ(req.headers["host"], "localhost:8686");
    EXPECT_EQ(req.headers["HOST"], "localhost:8686");
    EXPECT_EQ(req.headers["hOsT"], "localhost:8686");
    
    EXPECT_EQ(req.headers["Connection"], "keep-alive");
    EXPECT_EQ(req.headers["connection"], "keep-alive");
    EXPECT_EQ(req.headers["CONNECTION"], "keep-alive");
    EXPECT_EQ(req.headers["cOnNeCtIoN"], "keep-alive");

    EXPECT_EQ(req.headers.size(), 2);
}

/*
    @brief Check that all forms of parameter parsing is working properly

    Checks for
    - No params
    - Valid single param (param=value)
    - Valid multiple params (param1=value1&param2=value2)
    - Empty param value (param=)
    - Empty param value without '=' (param)
    - Trailing '?'
    - Trailing '&'
    - Mixed
*/
TEST(HttpMessageTest, HttpRequest_ParameterParsing) {

    // Test simple URL without parameters
    HttpRequest req1;
    req1.requestUrl = "/test";
    EXPECT_EQ(req1.requestUrl, "/test");
    EXPECT_EQ(req1.params.size(), 0);

    // Test URL with single parameter
    HttpRequest req2;
    std::stringstream ss2("GET /page?param=value HTTP/1.1\r\n\r\n");
    req2.ParseFrom(ss2);
    EXPECT_EQ(req2.requestUrl, "/page");
    EXPECT_EQ(req2.params.size(), 1);
    EXPECT_EQ(req2.params["param"], "value");

    // Test URL with multiple parameters
    HttpRequest req3;
    std::stringstream ss3("GET /index.html?a=1&b=2&c=3 HTTP/1.1\r\n\r\n");
    req3.ParseFrom(ss3);
    EXPECT_EQ(req3.requestUrl, "/index.html");
    EXPECT_EQ(req3.params.size(), 3);
    EXPECT_EQ(req3.params["a"], "1");
    EXPECT_EQ(req3.params["b"], "2");
    EXPECT_EQ(req3.params["c"], "3");

    // Test URL with empty parameter value
    HttpRequest req4;
    std::stringstream ss4("GET /search?q= HTTP/1.1\r\n\r\n");
    req4.ParseFrom(ss4);
    EXPECT_EQ(req4.requestUrl, "/search");
    EXPECT_EQ(req4.params.size(), 1);
    EXPECT_EQ(req4.params["q"], "");

    // Test URL with parameter without value
    HttpRequest req5;
    std::stringstream ss5("GET /toggle?dark HTTP/1.1\r\n\r\n");
    req5.ParseFrom(ss5);
    EXPECT_EQ(req5.requestUrl, "/toggle");
    EXPECT_EQ(req5.params.size(), 1);
    EXPECT_EQ(req5.params["dark"], "");

    // Test URL with trailing question mark
    HttpRequest req6;
    std::stringstream ss6("GET /page? HTTP/1.1\r\n\r\n");
    req6.ParseFrom(ss6);
    EXPECT_EQ(req6.requestUrl, "/page");
    EXPECT_EQ(req6.params.size(), 0);

    // Test URL with trailing ampersand
    HttpRequest req7;
    std::stringstream ss7("GET /page?a=1& HTTP/1.1\r\n\r\n");
    req7.ParseFrom(ss7);
    EXPECT_EQ(req7.requestUrl, "/page");
    EXPECT_EQ(req7.params.size(), 1);
    EXPECT_EQ(req7.params["a"], "1");

    // Test URL with mixed parameter types
    HttpRequest req8;
    std::stringstream ss8("GET /complex?a=1&b=&c&d=4 HTTP/1.1\r\n\r\n");
    req8.ParseFrom(ss8);
    EXPECT_EQ(req8.requestUrl, "/complex");
    EXPECT_EQ(req8.params.size(), 4);
    EXPECT_EQ(req8.params["a"], "1");
    EXPECT_EQ(req8.params["b"], "");
    EXPECT_EQ(req8.params["c"], "");
    EXPECT_EQ(req8.params["d"], "4");
}


// -- Test HttpResponse

/*
    @brief Check default constructor of HttpResponse
*/
TEST(HttpMessageTest, HttpResponse_DefaultConstructor) {

    HttpResponse res;
    
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusText, "OK");
    
    EXPECT_EQ(res.headers.size(), 0);
    EXPECT_EQ(res.body, "");
}

/*
    @brief Check parameterized constructor of HttpResponse
*/
TEST(HttpMessageTest, HttpResponse_ParameterizedConstructor) {
    
    HttpResponse res(
        HttpVersion::HTTP_1_1, 
        200, 
        "OK", 
        {{"Content-Type", "text/html"}}, 
        "<html></html>"
    );
    
    EXPECT_EQ(res.version, HttpVersion::HTTP_1_1);
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusText, "OK");
    
    EXPECT_EQ(res.headers.size(), 1);
    EXPECT_EQ(res.headers["Content-Type"], "text/html");
    
    EXPECT_EQ(res.body, "<html></html>");
}

/*
    @brief Check that the HttpResponse object can handle case insensitive headers
*/
TEST(HttpMessageTest, HttpResponse_CaseInsensitiveHeader) {
    
    HttpResponse res(
        HttpVersion::HTTP_1_1, 
        200, 
        "OK", 
        {
            {"Content-Type", "text/html"},
            {"Connection", "keep-alive"}
        }, 
        "<html></html>"
    );

    EXPECT_EQ(res.headers["Content-Type"], "text/html");
    EXPECT_EQ(res.headers["content-type"], "text/html");
    EXPECT_EQ(res.headers["CONTENT-TYPE"], "text/html");
    EXPECT_EQ(res.headers["CoNtEnT-TyPe"], "text/html");

    EXPECT_EQ(res.headers["Connection"], "keep-alive");
    EXPECT_EQ(res.headers["connection"], "keep-alive");
    EXPECT_EQ(res.headers["CONNECTION"], "keep-alive");
    EXPECT_EQ(res.headers["cOnNeCtIoN"], "keep-alive");

    EXPECT_EQ(res.headers.size(), 2);
}

// -- Formatters

/*
    @brief Formatter for HttpRequest
    This test checks if the HttpRequest object can be formatted correctly
*/
TEST(HttpMessageTest, HttpMethod_Formatter) {
    EXPECT_EQ(std::format("{}", HttpMethod::GET),             "GET");
    EXPECT_EQ(std::format("{}", HttpMethod::HEAD),            "HEAD");
    EXPECT_EQ(std::format("{}", HttpMethod::POST),            "POST");
    EXPECT_EQ(std::format("{}", HttpMethod::PUT),             "PUT");
    EXPECT_EQ(std::format("{}", HttpMethod::DELETE),          "DELETE");
    EXPECT_EQ(std::format("{}", HttpMethod::CONNECT),         "CONNECT");
    EXPECT_EQ(std::format("{}", HttpMethod::OPTIONS),         "OPTIONS");
    EXPECT_EQ(std::format("{}", HttpMethod::TRACE),           "TRACE");
    EXPECT_EQ(std::format("{}", HttpMethod::PATCH),           "PATCH");
    EXPECT_EQ(std::format("{}", HttpMethod::DEFAULT_INVALID), "INVALID");
}

/*
    @brief Formatter for HttpResponse
    This test checks if the HttpResponse object can be formatted correctly
*/
TEST(HttpMessageTest, HttpVersion_Formatter) {
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_1_0), "HTTP/1.0");
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_1_1), "HTTP/1.1");
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_2_0), "HTTP/2.0");
    EXPECT_EQ(std::format("{}", HttpVersion::DEFAULT_INVALID), "INVALID");
}
