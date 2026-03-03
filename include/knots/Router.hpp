#pragma once

#include <functional>
#include <map>
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

    HandlerFunction m_post;
    HandlerFunction m_get;
    HandlerFunction m_head;
    HandlerFunction m_put;
    HandlerFunction m_delete;
    HandlerFunction m_connect;
    HandlerFunction m_options;
    HandlerFunction m_trace;
    HandlerFunction m_patch;

    SegmentHandlerFunctions();

    const HandlerFunction& GetHandler(const HttpMethod method) const;
    void SetHandler(const HttpMethod method, const HandlerFunction& handler);
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
            case HttpMethod::POST:            return handlers.m_post != nullptr;
            case HttpMethod::GET:             return handlers.m_get != nullptr;
            case HttpMethod::HEAD:            return handlers.m_head != nullptr;
            case HttpMethod::PUT:             return handlers.m_put != nullptr;
            case HttpMethod::DELETE:          return handlers.m_delete != nullptr;
            case HttpMethod::CONNECT:         return handlers.m_connect != nullptr;
            case HttpMethod::OPTIONS:         return handlers.m_options != nullptr;
            case HttpMethod::TRACE:           return handlers.m_trace != nullptr;
            case HttpMethod::PATCH:           return handlers.m_patch != nullptr;
            case HttpMethod::DEFAULT_INVALID: break;
        }

        return false;
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
    std::map<
        std::string,
        SegmentHandlerFunctions
    > m_staticRoutes;

    std::shared_ptr<UrlSegment> m_dynamicRoutesTreeRoot;

    const UrlSegment* FindSegmentForRoute(HttpRequest& req) const;

public:
    Router();

    void AddRoute(
        const HttpMethod& method,
        std::string requestUrl, 
        const HandlerFunction& handler
    );

    const SegmentHandlerFunctions* FetchFunctionsForRoute(HttpRequest& req) const;

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