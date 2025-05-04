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

    EXPECT_FALSE(req.IsValid());
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
        "body"
    );
    
    EXPECT_EQ(req.method, HttpMethod::POST);
    EXPECT_EQ(req.requestUrl, "/test");
    EXPECT_EQ(req.version, HttpVersion::HTTP_1_1);
    
    EXPECT_EQ(req.headers.size(), 1);
    EXPECT_EQ(req.headers["Host"], "localhost:8686");
    
    EXPECT_EQ(req.body, "body");

    EXPECT_TRUE(req.IsValid());
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
        "body"
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
    EXPECT_TRUE(req.IsValid());
}


// -- Test HttpResponse

/*
    @brief Check default constructor of HttpResponse
*/
TEST(HttpMessageTest, HttpResponse_DefaultConstructor) {

    HttpResponse res;
    
    EXPECT_EQ(res.version, HttpVersion::DEFAULT_INVALID);
    EXPECT_EQ(res.statusCode, 0);
    EXPECT_EQ(res.statusText, "");
    
    EXPECT_EQ(res.headers.size(), 0);
    EXPECT_EQ(res.body, "");

    EXPECT_FALSE(res.IsValid());
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

    EXPECT_TRUE(res.IsValid());
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
    
    EXPECT_TRUE(res.IsValid());
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
