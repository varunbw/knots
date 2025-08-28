#ifndef KNOTS_HTTPMESSAGE_HPP
#define KNOTS_HTTPMESSAGE_HPP

#include <algorithm>
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

using Header = std::pair<std::string, std::string>;

/*
    Case insensitive hash function for string comparison
    Used for the headers map in HttpRequest and HttpResponse

    @note This does not actually store a lower-case key in the hashmap, the key is just treated
          as if it were lower-case completely
*/
struct CaseInsensitiveHash {
    size_t operator() (const std::string& key) const {
        std::string lower = key;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return std::hash<std::string>{}(lower);
    }
};

/*
    Case insensitive equal function for string comparison
    Used for the headers map in HttpRequest and HttpResponse
*/
struct CaseInsensitiveEqual {
    bool operator() (const std::string& left, const std::string& right) const {
        return std::equal(
            left.begin(), left.end(), right.begin(), right.end(),
            [] (const char a, const char b) {
                return ::tolower(a) == ::tolower(b);
            }
        );
    }
};

struct HttpRequest {

    HttpMethod method;
    std::string requestUrl;
    HttpVersion version;

    std::unordered_map<
        std::string,
        std::string,
        CaseInsensitiveHash,
        CaseInsensitiveEqual
    > headers;

    std::string body;

    HttpRequest() : 
        method(HttpMethod::DEFAULT_INVALID),
        requestUrl{},
        version(HttpVersion::DEFAULT_INVALID),
        headers{},
        body{}
    {}
    
    HttpRequest(
        const HttpMethod method,
        const std::string_view requestUrl,
        const HttpVersion version,
        const std::unordered_map<
            std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual
        > headers,
        const std::string_view body
    ) :
        method(method),
        requestUrl(requestUrl),
        version(version),
        headers(headers),
        body(body)
    {}

    bool IsValid() const noexcept;
    void PrintMessage() const;
    bool ParseFrom(std::stringstream&);
};


struct HttpResponse {

    HttpVersion version;
    short int statusCode;
    std::string statusText;

    std::unordered_map<
        std::string,
        std::string,
        CaseInsensitiveHash,
        CaseInsensitiveEqual
    > headers;

    std::string body;

    HttpResponse();

    HttpResponse(
        const HttpVersion& version,
        const short int statusCode,
        const std::string_view statusText,
        const std::unordered_map<
            std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual
        > headers,
        const std::string_view body
    ) :
        version(version),
        statusCode(statusCode),
        statusText(statusText),
        headers(headers),
        body(body)
    {}

    bool IsValid() const noexcept;
    void PrintMessage() const;
    void SetStatus(const int statusCode);
    std::string Serialize() const;
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