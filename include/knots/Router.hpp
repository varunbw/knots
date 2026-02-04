#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "knots/HttpMessage.hpp"

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

struct SegmentHandlerFunctions {
    std::optional<HandlerFunction> m_post;
    std::optional<HandlerFunction> m_get;
    std::optional<HandlerFunction> m_head;
    std::optional<HandlerFunction> m_put;
    std::optional<HandlerFunction> m_delete;
    std::optional<HandlerFunction> m_connect;
    std::optional<HandlerFunction> m_options;
    std::optional<HandlerFunction> m_trace;
    std::optional<HandlerFunction> m_patch;

    const std::optional<HandlerFunction> GetHandler(const HttpMethod method) const;
    void SetHandler(const HttpMethod method, const HandlerFunction handler);
};

struct UrlSegment {
    std::string value;

    SegmentHandlerFunctions handlers;
    std::vector<std::shared_ptr<UrlSegment>> next;

    UrlSegment() :
        value{},
        handlers{},
        next{}
    {}

    UrlSegment(const std::string& value) :
        value(value),
        handlers{},
        next{}
    {}

    bool isDynamic() const {
        return value[0] == '{' && value.back() == '}';
    }

    bool IsEndpoint(const HttpMethod& method) const {
        switch (method) {
            case HttpMethod::POST:            return handlers.m_post.has_value();
            case HttpMethod::GET:             return handlers.m_get.has_value();
            case HttpMethod::HEAD:            return handlers.m_head.has_value();
            case HttpMethod::PUT:             return handlers.m_put.has_value();
            case HttpMethod::DELETE:          return handlers.m_delete.has_value();
            case HttpMethod::CONNECT:         return handlers.m_connect.has_value();
            case HttpMethod::OPTIONS:         return handlers.m_options.has_value();
            case HttpMethod::TRACE:           return handlers.m_trace.has_value();
            case HttpMethod::PATCH:           return handlers.m_patch.has_value();
            case HttpMethod::DEFAULT_INVALID: break;
        }
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

    const HandlerFunction* FetchRoute(HttpRequest& req) const;

    void DebugDFS() const;

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