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

/*
    @brief Check default constructor of HttpResponse
*/
TEST(HttpResponseTest, DefaultConstructor) {

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
TEST(HttpResponseTest, ParameterizedConstructor) {
    
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
    EXPECT_EQ(res.GetHeader("Content-Type"), "text/html");
    
    EXPECT_EQ(res.body, "<html></html>");
}

/*
    @brief Check that the HttpResponse object can handle case insensitive headers
*/
TEST(HttpResponseTest, CaseInsensitiveHeader) {
    
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

    EXPECT_EQ(res.GetHeader("Content-Type"), "text/html");
    EXPECT_EQ(res.GetHeader("content-type"), "text/html");
    EXPECT_EQ(res.GetHeader("CONTENT-TYPE"), "text/html");
    EXPECT_EQ(res.GetHeader("CoNtEnT-TyPe"), "text/html");

    EXPECT_EQ(res.GetHeader("Connection"), "keep-alive");
    EXPECT_EQ(res.GetHeader("connection"), "keep-alive");
    EXPECT_EQ(res.GetHeader("CONNECTION"), "keep-alive");
    EXPECT_EQ(res.GetHeader("cOnNeCtIoN"), "keep-alive");

    EXPECT_EQ(res.headers.size(), 2);
}

// -- Formatters

/*
    @brief Formatter for HttpRequest
    This test checks if the HttpRequest object can be formatted correctly
*/
TEST(HttpResponseTest, HttpMethod_Formatter) {
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
TEST(HttpResponseTest, HttpVersion_Formatter) {
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_1_0), "HTTP/1.0");
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_1_1), "HTTP/1.1");
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_2_0), "HTTP/2.0");
    EXPECT_EQ(std::format("{}", HttpVersion::DEFAULT_INVALID), "INVALID");
}
