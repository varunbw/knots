#ifndef KNOTS_HTTPMESSAGE_HPP
#define KNOTS_HTTPMESSAGE_HPP

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

    bool IsValid() const;
    bool HasHeader(const std::string& header) const;
};


#endif