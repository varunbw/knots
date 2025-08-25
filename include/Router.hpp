#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>

#include "HttpMessage.hpp"

/*
    A combination of a HTTP Method (GET, POST, etc.) and the request URL
    This will act as the key to the map in the router later on to fetch the
    corresponding handler function
*/
struct Route {
    HttpMethod method;
    std::string requestUrl;

    bool operator== (const Route& other) const {
        return method == other.method && requestUrl == other.requestUrl;
    }
};

/*
    Custom hasher for the Routes struct
*/
struct RouteHasher {
    std::size_t operator() (const Route& route) const {
        return std::hash<HttpMethod>()(route.method) ^
               std::hash<std::string>()(route.requestUrl);
    }
};

/*
    Alias for the handler functions
*/
using HandlerFunction = std::function<void(
    const HttpRequest&, HttpResponse&
)>;

/*
    `m_routes` stores a key-value pairing of Routes to their corresponding
    handler functions

    Add routes to the router using `AddRoute()`, and get the handler function
    using `FetchRoute()`
*/
class Router {
private:
    std::unordered_map<Route, HandlerFunction, RouteHasher> m_routes;

public:
    void AddRoute(
        const HttpMethod& method,
        const std::string& requestUrl, 
        const HandlerFunction& handler
    );

    void AddRoute(
        const Route& route,
        const HandlerFunction& handler
    );

    const HandlerFunction* FetchRoute(
        const HttpMethod& method,
        const std::string& requestUrl
    ) const;

    const HandlerFunction* FetchRoute(
        const Route& route
    ) const;
};