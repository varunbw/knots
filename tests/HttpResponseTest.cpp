#include <gtest/gtest.h>

#include "HttpMessage.hpp"


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

/*
    @brief Check that the API sets `statusCode` and `statusText` correctly
*/
TEST(HttpResponseTest, SetStatusAPI) {

    const std::map<int, std::string> statusText = {
        {100, "Continue"},
        {101, "Switching Protocols"},
        // {102, "Processing"}, // Deprecated
        {103, "Early Hints"},

        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"}, // WebDAV
        {208, "Already Reported"}, // WebDAV
        {226, "IM Used"},
        
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        // {305, "Use Proxy"}, // Deprecated
        // {306, ""}, // Unused
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},

        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"}, // lmao
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"}, // WebDAV
        {423, "Locked"}, // WebDAV
        {424, "Failed Dependency"}, // WebDAV
        {425, "Too Early"}, // Experimental
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},

        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"}, // WebDAV
        {508, "Loop Detected"}, // WebDAV
        {510, "Not Extended"},
        {511, "Network Authentication Required"}
    };
    
    HttpResponse res;
    for (const auto& [code, text] : statusText) {
        res.SetStatus(code);

        EXPECT_EQ(res.statusCode, code);
        EXPECT_EQ(res.statusText, text);
    }
}

/*
    @brief Check that the APIs insert, fetch, and delete headers correctly
    SetHeader
    - Header is inserted properly
    GetHeader
    - If key if found, value should be returned
    - Else, `std::nullopt` should be returned
    DeleteHeader
    - Remove the header properly
*/
TEST(HttpResponseTest, HeadersAPI) {

    HttpResponse res;
    res.SetHeader("Content-Type", "text/html");
    res.SetHeader("Connection", "close");

    EXPECT_EQ(res.headers.size(), 2);
    EXPECT_EQ(res.headers["Content-Type"], "text/html");
    EXPECT_EQ(res.GetHeader("Content-Type"), "text/html");
    EXPECT_EQ(res.headers["Connection"], "close");
    EXPECT_EQ(res.GetHeader("Connection"), "close");
    EXPECT_EQ(res.GetHeader("Not-Present-Header"), std::nullopt);

    res.DeleteHeader("Content-Type");
    EXPECT_EQ(res.GetHeader("Content-Type"), std::nullopt);
    EXPECT_EQ(res.GetHeader("Connection"), "close");
    EXPECT_EQ(res.headers.size(), 1);

    res.DeleteHeader("Connection");
    EXPECT_EQ(res.GetHeader("Connection"), std::nullopt);
    EXPECT_EQ(res.headers.size(), 0);
}

/*
    @brief Check both overloads of the API with the Content-Length header auto-set feature

    - Body is set and Content-Length header is set automatically
    - Body is set but Content-Length header is not added
    - Body is set and original string is empty with std::move()
    - Body is set, original string is empty with std::move(),
      and Content-Length header is omitted
*/
TEST(HttpResponseTest, SetBodyAPI) {

    HttpResponse res;
    std::string str(1024, '0');
    const std::string constStr(1024, '0');

    // Basic call
    res.SetBody(str);
    EXPECT_EQ(res.body, str);
    EXPECT_EQ(res.GetHeader("Content-Length"), std::to_string(str.size()));
    EXPECT_EQ(str, constStr);

    // Don't set `Content-Length` header
    res = HttpResponse();
    constexpr bool setContentLengthHeader = false;
    res.SetBody(str, setContentLengthHeader);
    EXPECT_EQ(res.body, str);
    EXPECT_EQ(res.GetHeader("Content-Length"), std::nullopt);
    EXPECT_EQ(str, constStr);

    // Using std::move()
    res = HttpResponse();
    res.SetBody(std::move(str));
    EXPECT_EQ(res.body, constStr);
    EXPECT_EQ(res.GetHeader("Content-Length"), std::to_string(constStr.size()));
    EXPECT_EQ(str, "");
    EXPECT_EQ(str.size(), 0);

    // Using std::move() and not set `Content-Length` header
    str = constStr;
    res = HttpResponse();
    res.SetBody(std::move(str), setContentLengthHeader);
    EXPECT_EQ(res.body, constStr);
    EXPECT_EQ(res.GetHeader("Content-Length"), std::nullopt);
    EXPECT_EQ(str, "");
    EXPECT_EQ(str.size(), 0);
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
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_1_0),        "HTTP/1.0");
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_1_1),        "HTTP/1.1");
    EXPECT_EQ(std::format("{}", HttpVersion::HTTP_2_0),        "HTTP/2.0");
    EXPECT_EQ(std::format("{}", HttpVersion::DEFAULT_INVALID), "INVALID");
}
