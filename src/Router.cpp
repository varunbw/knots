#include "Router.hpp"

// Big brain comments up ahead

/*
    @brief Add a route to the Router
    @param method HTTP Method
    @param requestUrl Request URL
    @param handler The handler function
*/
void Router::AddRoute(
    const HttpMethod& method,
    const std::string& requestUrl,
    const HandlerFunction& handler
) {
    m_routes[Route(method, requestUrl)] = handler;
    return;
}

/*
    @brief Add a route to the Router
    @param route The route
    @param handler The handler function
*/
void Router::AddRoute(
    const Route& route,
    const HandlerFunction& handler
) {
    m_routes[route] = handler;
    return;
}

/*
    @brief Get a const pointer to the handler function for the given route
    @param method HTTP Method
    @param requestUrl Request URL

    @return const pointer to handler function
*/
const HandlerFunction* Router::FetchRoute(
    const HttpMethod& method,
    const std::string& requestUrl
) const {
    auto it = m_routes.find(Route(method, requestUrl));
    if (it == m_routes.end()) {
        return nullptr;
    }

    return &(it->second);
}

/*
    @brief Get a const pointer to the handler function for the given route
    @param route The route

    @return const pointer to handler function
*/
const HandlerFunction* Router::FetchRoute(
    const Route& route
) const {
    auto it = m_routes.find(route);
    if (it == m_routes.end()) {
        return nullptr;
    }

    return &(it->second);
} 


// Individual functions for request types
void Router::Post(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::POST, requestUrl, handler);
    return;
};

void Router::Get(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::GET, requestUrl, handler);
    return;
};

void Router::Head(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::HEAD, requestUrl, handler);
    return;
};

void Router::Put(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::PUT, requestUrl, handler);
    return;
};

void Router::Delete(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::DELETE, requestUrl, handler);
    return;
};

void Router::Connect(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::CONNECT, requestUrl, handler);
    return;
};

void Router::Options(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::OPTIONS, requestUrl, handler);
    return;
};

void Router::Trace(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::TRACE, requestUrl, handler);
    return;
};

void Router::Patch(const std::string& requestUrl, const HandlerFunction& handler) {
    this->AddRoute(HttpMethod::PATCH, requestUrl, handler);
    return;
};

