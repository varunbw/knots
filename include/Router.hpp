#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "HttpMessage.hpp"

/*
    Alias for the handler functions
*/
using HandlerFunction = std::function<
    void(const HttpRequest&, HttpResponse&)
>;

/*
    A combination of a HTTP Method (GET, POST, etc.) and the request URL
    This will act as the key to the map in the router later on to fetch the
    corresponding handler function
*/
struct Route {
    HttpMethod method;
    std::string requestUrl;

    Route() :
        method(HttpMethod::DEFAULT_INVALID),
        requestUrl{}
    {};

    Route(const HttpMethod method, const std::string& requestUrl) :
        method(method),
        requestUrl(requestUrl)
    {}

    constexpr bool operator== (const Route& other) const {
        return method == other.method && requestUrl == other.requestUrl;
    }
};

struct UrlSegment {
    HttpMethod method;
    std::string segment;

    bool isEndpoint;

    std::optional<HandlerFunction> handler;
    std::vector<std::shared_ptr<UrlSegment>> next;

    UrlSegment() :
        method(HttpMethod::DEFAULT_INVALID),
        segment{},
        isEndpoint(false),
        handler{},
        next{}
    {}

    UrlSegment(const HttpMethod& method, const std::string& segment) :
        method(method),
        segment(segment),
        isEndpoint(false),
        handler{},
        next{}
    {}

    constexpr bool operator== (const UrlSegment& other) const {
        return method == other.method && segment == other.segment;
    }

    constexpr bool isDynamic() const {
        return segment[0] == '{' && segment.back() == '}';
    }
};



/*
    `m_routes` stores a key-value pairing of Routes to their corresponding
    handler functions

    Add routes to the router using `AddRoute()`, and get the handler function
    using `FetchRoute()`
*/
class Router {
private:
    std::shared_ptr<UrlSegment> m_root;

    std::shared_ptr<UrlSegment> FindSegmentForRoute(HttpRequest& req) const;

public:
    Router();

    void AddRoute(
        const HttpMethod& method,
        const std::string& requestUrl, 
        const HandlerFunction& handler
    );

    const HandlerFunction* FetchRoute(
        HttpRequest& req
    ) const;

    // Individual functions for request types
    void Post(const std::string& requestUrl, const HandlerFunction& handler);
    void Get(const std::string& requestUrl, const HandlerFunction& handler);
    void Head(const std::string& requestUrl, const HandlerFunction& handler);
    void Put(const std::string& requestUrl, const HandlerFunction& handler);
    void Delete(const std::string& requestUrl, const HandlerFunction& handler);
    void Connect(const std::string& requestUrl, const HandlerFunction& handler);
    void Options(const std::string& requestUrl, const HandlerFunction& handler);
    void Trace(const std::string& requestUrl, const HandlerFunction& handler);
    void Patch(const std::string& requestUrl, const HandlerFunction& handler);
};