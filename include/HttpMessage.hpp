#ifndef KNOTS_HTTPMESSAGE_HPP
#define KNOTS_HTTPMESSAGE_HPP

#include <format>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>

enum class HttpMethod {
    GET = 1,
    HEAD = 2,
    POST = 3,
    PUT = 4,
    DELETE = 5,
    CONNECT = 6,
    OPTIONS = 7,
    TRACE = 8,
    PATCH = 9,

    DEFAULT_INVALID = 10
};

enum class HttpVersion {
    HTTP_1_0 = 1,
    HTTP_1_1 = 2,
    HTTP_2_0 = 3,

    DEFAULT_INVALID = 4
};


struct HttpRequest {

    HttpMethod method;
    std::string requestUrl;
    HttpVersion version;

    std::unordered_map<std::string, std::string> headers;

    std::string body;

    HttpRequest() : 
        method(HttpMethod::DEFAULT_INVALID),
        requestUrl{},
        version(HttpVersion::HTTP_1_0),
        headers{},
        body{}
    {}
    
    HttpRequest(
        const HttpMethod method,
        const std::string_view requestUrl,
        const HttpVersion version,
        const std::unordered_map<std::string, std::string> headers,
        const std::string_view body
    ) :
        method(method),
        requestUrl(requestUrl),
        version(version),
        headers(headers),
        body(body)
    {}

    bool IsValid() const noexcept;
    auto FindHeader(std::string header) const;
    void PrintMessage() const;
};



struct HttpResponse {

    HttpVersion version;
    short int statusCode;
    std::string statusText;

    std::unordered_map<std::string, std::string> headers;

    std::string body;

    HttpResponse() :
        version(HttpVersion::DEFAULT_INVALID),
        statusCode{},
        statusText{},
        headers{},
        body{}
    {}

    HttpResponse(
        const HttpVersion& version,
        const short int statusCode,
        const std::string_view statusText,
        const std::unordered_map<std::string, std::string> headers,
        const std::string_view body
    ) :
        version(version),
        statusCode(statusCode),
        statusText(statusText),
        headers(headers),
        body(body)
    {}

    bool IsValid() const noexcept;
    auto FindHeader(std::string header) const;
    void PrintMessage() const;
};


/*
    Special formatter to print the HttpMethod and HttpVersion enums
    Not really needed to print, but it makes life easier when printing with std::format

    @note This is a C++20 feature
*/
template<>
struct std::formatter<HttpMethod> : std::formatter<std::string> {
    auto format(const HttpMethod& method, std::format_context& ctx) const {
        std::string_view name;
        switch (method) {
            case HttpMethod::GET: name = "GET"; break;
            case HttpMethod::HEAD: name = "HEAD"; break;
            case HttpMethod::POST: name = "POST"; break;
            case HttpMethod::PUT: name = "PUT"; break;
            case HttpMethod::DELETE: name = "DELETE"; break;
            case HttpMethod::CONNECT: name = "CONNECT"; break;
            case HttpMethod::OPTIONS: name = "OPTIONS"; break;
            case HttpMethod::TRACE: name = "TRACE"; break;
            case HttpMethod::PATCH: name = "PATCH"; break;
            default: name = "INVALID"; break;
        }
        return std::formatter<std::string>::format(std::string(name), ctx);
    }
};

template<>
struct std::formatter<HttpVersion> : std::formatter<std::string> {
    auto format(const HttpVersion& version, std::format_context& ctx) const {
        std::string_view name;
        switch (version) {
            case HttpVersion::HTTP_1_0: name = "HTTP/1.0"; break;
            case HttpVersion::HTTP_1_1: name = "HTTP/1.1"; break;
            case HttpVersion::HTTP_2_0: name = "HTTP/2.0"; break;
            default: name = "INVALID"; break;
        }
        return std::formatter<std::string>::format(std::string(name), ctx);
    }
};


#endif