#pragma once

#include <algorithm>
#include <format>
#include <map>
#include <optional>
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
        std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual
    > headers;

    std::string body;
    
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> routeParams;

    HttpRequest() : 
        method(HttpMethod::DEFAULT_INVALID),
        requestUrl{},
        version(HttpVersion::DEFAULT_INVALID),
        headers{},
        body{},
        queryParams{},
        routeParams{}
    {}

    HttpRequest(
        const HttpMethod method,
        const std::string_view requestUrl,
        const HttpVersion version,
        const std::unordered_map<
            std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual
        > headers,
        const std::string_view body,
        const std::unordered_map<std::string, std::string> queryParams,
        const std::unordered_map<std::string, std::string> routeParams
    ) :
        method(method),
        requestUrl(requestUrl),
        version(version),
        headers(headers),
        body(body),
        queryParams(queryParams),
        routeParams(routeParams)
    {}

    /*
        @brief Print a formatted HttpRequest object to the console

        Example:
        ------- HTTP Request -------
        [METHOD] : GET
        [URL]    : /
        [VERSION]: HTTP/1.1

        HEADERS
        User-Agent: curl/8.14.1
        Host: localhost:8600

        PARAMETERS

        BODY

        ------- End Request -------
    */
    void PrintMessage() const;

    /*
        @brief Parse the HttpRequest message
        @param ss Message in stringstream format

        @return The request in a `HttpRequest` struct
    */
    bool ParseFrom(std::stringstream& ss);

    /*
        @brief Getter for header field
        @param key Key of the associated value to fetch
        @return The value associated with the key if found, else `std::nullopt`
    */
    std::optional<std::string> GetHeader(const std::string& key) const;
    /*
        @brief Getter for queryParams field
        @param key Key of the associated value to fetch
        @return The value associated with the key if found, else `std::nullopt`
    */
    std::optional<std::string> GetQueryParam(const std::string& key) const;
    /*
        @brief Getter for routeParams field
        @param key Key of the associated value to fetch
        @return The value associated with the key if found, else `std::nullopt`
    */
    std::optional<std::string> GetRouteParam(const std::string& key) const;
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

    HttpResponse() :
        version(HttpVersion::HTTP_1_1),
        statusCode(200),
        statusText("OK"),
        headers{},
        body{}
    {}

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

    /*
        @brief Print a formatted HttpResponse object to the console

        Example message:
        ------- HTTP Response -------
          [VERSION]     : HTTP/1.1
          [STATUS CODE] : 200
          [STATUS TEXT] : OK

        HEADERS
          Content-Type: text/html
          Content-Length: 16

        BODY
        0000000000000000
        ------- End Response -------
    */  
    void PrintMessage() const;

    /*
        @brief Set the provided status and its corresponding text
        @param statusCode status to set
    */
    void SetStatus(const int statusCode);
    /*
        @brief Set the given header
        @param key key
        @param value value
    */
    void SetHeader(const std::string& key, const std::string& value);

    /*
        @brief Get the header value
        @param key Key to the header
    
        @return The header value if it exists, else `std::nullopt`
    */
    std::optional<std::string> GetHeader(const std::string& key) const;

    /*
        @brief Delete the header with key `key`
        @param key key
    */
    void DeleteHeader(const std::string& key);

    /*
        @brief Set provided body, and the "Content-Length" header automatically
        @param body Body
        @param setContentLengthHeader Whether to automatically set the "Content-Length" header as per
               the length of the body or not. Set to true by default if no parameter is passed
    */
    void SetBody(const std::string& body, const bool setContentLengthHeader = true);
    void SetBody(std::string&& body, const bool setContentLengthHeader = true);

    /*
        @brief Serialize the object into a `std::string` according to the standard HTTP response format
    */
    std::string Serialize() const;
};


/*
    Special formatter to print the HttpMethod enum

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

/*
    Special formatter to print the HttpVersion enum

    @note This is a C++20 feature
*/
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