#include <stack>

#include "Router.hpp"
#include "Utils.hpp"


Router::Router() {
    m_root = std::make_shared<UrlSegment>();
    m_root->segment = ""; 
    return;
}

std::vector<UrlSegment> BreakRouteIntoSegments(const Route& route) {

    std::vector<UrlSegment> res;

    const HttpMethod& method = route.method;
    const std::string& requestUrl = route.requestUrl;

    size_t findFromPosition = 0;
    const size_t urlLength = requestUrl.size();

    // The root endpoint should be before everything
    res.push_back(UrlSegment(HttpMethod::DEFAULT_INVALID, ""));
    res.push_back(UrlSegment(method, "/"));

    if (route.requestUrl.size() == 1) {
        res.back().isEndpoint = true;
        return res;
    }

    while (findFromPosition < urlLength) {
        const size_t left = requestUrl.find('/', findFromPosition);
        const size_t right = [requestUrl, urlLength, left] () {
            const size_t res = requestUrl.find('/', left + 1);

            return (
                res == std::string::npos ? 
                urlLength :
                res - 1
            );
        } ();

        res.push_back(UrlSegment(method, requestUrl.substr(left + 1, right - left)));

        UrlSegment& lastSegment = res.back();
        lastSegment.method = route.method;

        findFromPosition = right + 1;
    }

    res.back().isEndpoint = true;
    return res;
}

std::shared_ptr<UrlSegment> Router::FindSegmentForRoute(HttpRequest& req) const {

    std::shared_ptr<UrlSegment> currNode = m_root;

    // const Route route = Route(
    //     req.method,
    //     [req] () {
    //         return req.requestUrl.back() == '/' ?
    //             req.requestUrl :
    //             req.requestUrl + "/";
    //     } ()
    // );
    const Route route = Route(req.method, req.requestUrl);
    Route currRoute(req.method, "");

    std::vector<UrlSegment> segments = BreakRouteIntoSegments(route);
    const size_t segmentsSize = segments.size();

    for (size_t i = 0; i < segmentsSize; i++) {
        // Substitute params for dynamic routes
        if (currNode->isDynamic()) {
            const std::string key = [currNode] () {
                std::string res = currNode->segment;
                res.pop_back();
                res.erase(res.begin());
                return res;
            } ();

            const std::string value = segments[i].segment;
            req.routeParams[key] = value;
        }

        currRoute.method = currNode->method;
        Log::Info(std::format(
            "croute: {}, {}",
            currRoute.method, currRoute.requestUrl
        ));
        Log::Info(std::format(
            "route:  {}, {}",
            route.method, route.requestUrl
        ));

        if (currRoute == route) {
            Log::Success(std::format(
                "route `{}` `{}`", 
                currRoute.method, currRoute.requestUrl
            ));
            Log::Success(std::format(
                "node `{}` `{}`",
                route.method, route.requestUrl
            ));
            return currNode;
        }

        // If a route is not found even at the last segment, there is no matching route
        if (i == segmentsSize - 1) {
            return nullptr;
        }

        bool nextStaticNodeFound = false;
        bool nextDynamicNodeFound = false;

        // Look for a static node
        for (const std::shared_ptr<UrlSegment>& nextNode: currNode->next) {
            // If looking for the last segment, match the method as well
            // We do not care for methods for segments before this one as they're not
            // the endpoint we want
            if (i == segmentsSize - 2) {
                if (
                    nextNode->segment == segments[i + 1].segment &&
                    nextNode->method == segments[i + 1].method &&
                    nextNode->isEndpoint
                ) {
                    currNode = nextNode;
                    nextStaticNodeFound = true;
                    if (currRoute.requestUrl.back() != '/') {
                        currRoute.requestUrl += "/";
                    }
                    currRoute.requestUrl += currNode->segment;
                    break;
                }
            }
            else {
                if (nextNode->segment == segments[i + 1].segment) {
                    currNode = nextNode;
                    nextStaticNodeFound = true;
                    if (currRoute.requestUrl.back() != '/') {
                        currRoute.requestUrl += "/";
                    }
                    currRoute.requestUrl += currNode->segment;
                    break;
                }
            }
        }

        // Look for a dynamic node
        if (nextStaticNodeFound == false) {
            for (const std::shared_ptr<UrlSegment>& nextNode: currNode->next) {

                if (i == segmentsSize - 2) {
                    if (
                        nextNode->isDynamic() &&
                        nextNode->method == route.method
                    ) {
                        currNode = nextNode;
                        nextDynamicNodeFound = true;
                        if (currRoute.requestUrl.back() != '/') {
                            currRoute.requestUrl += "/";
                        }
                        currRoute.requestUrl += segments[i + 1].segment;
                        break;
                    }
                }
                else {
                    if (nextNode->isDynamic()) {
                        currNode = nextNode;
                        nextDynamicNodeFound = true;
                        if (currRoute.requestUrl.back() != '/') {
                            currRoute.requestUrl += "/";
                        }
                        currRoute.requestUrl += segments[i + 1].segment;
                        break;
                    }
                }
            }

            // Neither a static or dynamic route from here onwards was found
            if (nextDynamicNodeFound == false) {
            }
        }
    }

    return nullptr;
}

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

    Route route(method, requestUrl);

    std::shared_ptr<UrlSegment> prevNode = nullptr;
    std::shared_ptr<UrlSegment> currNode = m_root;

    std::vector<UrlSegment> segments = BreakRouteIntoSegments(route);
    const size_t segmentsSize = segments.size();

    for (size_t i = 0; i < segmentsSize; i++) {
        // If current node does not exist, create and link it
        if (currNode == nullptr && prevNode != nullptr) {
            currNode = std::make_shared<UrlSegment>(segments[i]);
            prevNode->next.push_back(currNode);
        }

        bool nextNodeExists = false;
        for (const std::shared_ptr<UrlSegment>& nextNode : currNode->next) {

            if (i == segmentsSize - 1) {
                break;
            }

            if (i == segmentsSize - 2) {
                if (
                    nextNode->segment == segments[i + 1].segment &&
                    nextNode->method == segments[i + 1].method
                ) {
                    nextNodeExists = true;
                    prevNode = currNode;
                    currNode = nextNode;
                }
            }
            else {
                if (nextNode->segment == segments[i + 1].segment) {
                    nextNodeExists = true;
                    prevNode = currNode;
                    currNode = nextNode;
                    break;
                }
            }
        }

        if (nextNodeExists == false) {
            prevNode = currNode;
            currNode = nullptr;
        }
    }

    prevNode->isEndpoint = true;
    prevNode->handler = handler;
    return;
}


/*
    @brief Get a const pointer to the handler function for the given route
    @param method HTTP Method
    @param requestUrl Request URL

    @return const pointer to handler function
*/
// todo Fix this to remove raw pointer
const HandlerFunction* Router::FetchRoute(
    HttpRequest& req
) const {
    std::shared_ptr<UrlSegment> segment = FindSegmentForRoute(req);

    // Log::Info(std::format(
    //     "m_root: `{}`, `{}`",
    //     m_root->method, m_root->segment
    // ));

    // for (auto ele : m_root->next) {
    //     Log::Info(std::format(
    //         "m_root next: `{}`, `{}`",
    //         ele->method, ele->segment
    //     ));
    // }

    return (segment && segment->handler.has_value()) ?
        &segment->handler.value() :
        nullptr;
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
