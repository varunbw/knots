#include "knots/Router.hpp"
#include "knots/Utils.hpp"

SegmentHandlerFunctions::SegmentHandlerFunctions() :
    m_post(nullptr),
    m_get(nullptr),
    m_head(nullptr),
    m_put(nullptr),
    m_delete(nullptr),
    m_connect(nullptr),
    m_options(nullptr),
    m_trace(nullptr),
    m_patch(nullptr)
{}

const HandlerFunction& SegmentHandlerFunctions::GetHandler(const HttpMethod method) const {
    switch (method) {
        case HttpMethod::POST:            return m_post;
        case HttpMethod::GET:             return m_get;
        case HttpMethod::HEAD:            return m_head;
        case HttpMethod::PUT:             return m_put;
        case HttpMethod::DELETE:          return m_delete;
        case HttpMethod::CONNECT:         return m_connect;
        case HttpMethod::OPTIONS:         return m_options;
        case HttpMethod::TRACE:           return m_trace;
        case HttpMethod::PATCH:           return m_patch;
        case HttpMethod::DEFAULT_INVALID: break;
    }

    throw std::invalid_argument(MakeErrorMessage(
        "Invalid HttpMethod passed when querying segment for handler function")
    );
}

void SegmentHandlerFunctions::SetHandler(const HttpMethod method, const HandlerFunction& handler) {

    switch (method) {
        case HttpMethod::POST:            m_post = handler;    break;
        case HttpMethod::GET:             m_get = handler;     break;
        case HttpMethod::HEAD:            m_head = handler;    break;
        case HttpMethod::PUT:             m_put = handler;     break;
        case HttpMethod::DELETE:          m_delete = handler;  break;
        case HttpMethod::CONNECT:         m_connect = handler; break;
        case HttpMethod::OPTIONS:         m_options = handler; break;
        case HttpMethod::TRACE:           m_trace = handler;   break;
        case HttpMethod::PATCH:           m_patch = handler;   break;
        case HttpMethod::DEFAULT_INVALID: break;
    }

    return;
}

Router::Router() {
    // Make an empty root segment
    m_dynamicRoutesTreeRoot = std::make_shared<UrlSegment>(
        "/"
    );

    return;
};

std::vector<UrlSegment> BreakRouteIntoSegments(const std::string& requestUrl) {

    std::vector<UrlSegment> res;

    size_t findFromPosition = 0;
    const size_t urlLength = requestUrl.size();

    // The root endpoint should be before everything
    res.push_back(UrlSegment("/"));

    if (requestUrl.size() == 1) {
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

        res.push_back(UrlSegment(requestUrl.substr(left + 1, right - left)));

        findFromPosition = right + 1;
    }

    // In case of trailing `/`s in the URL, a blank segment is inserted
    if (res.back().value == "") {
        res.pop_back();
    }

    return res;
}

void SanitizeURL(std::string& url) {
    // Erase any trailing '/'s
    // This project will treat the following two routes as the same: "/users", "/users/"
    while (url.size() != 1 && url.back() == '/') {
        url.pop_back();
    }

    return;
}

bool IsRouteStatic(const std::string& requestUrl) {
    return requestUrl.find('{') == std::string::npos;
}

void Router::AddRoute(
    const HttpMethod& method,
    std::string requestUrl,
    const HandlerFunction& handler
) {

    SanitizeURL(requestUrl);

    const Route routeToAdd(method, requestUrl);

    const std::vector<UrlSegment> routeSegments = BreakRouteIntoSegments(routeToAdd.requestUrl);
    const size_t numSegments = routeSegments.size();

    if (IsRouteStatic(routeToAdd.requestUrl)) {
        // Insert it into the static table
        m_staticRoutes.insert(std::make_pair(
            routeToAdd.requestUrl,
            SegmentHandlerFunctions()
        ));

        m_staticRoutes
            .at(routeToAdd.requestUrl)
            .SetHandler(routeToAdd.method, handler);

        return;
    }

    std::shared_ptr<UrlSegment> prevNode = nullptr;
    std::shared_ptr<UrlSegment> currNode = m_dynamicRoutesTreeRoot;

    /*
        We don't need to start from the root node as it'll always be present in the router
    
        At all points in this loop, `currNode` will be pointing to
        the parent (if the link exists) of `routeSegments[i]`
        If this link does not exist, it's created accordingly
    */
    for (size_t i = 1; i < numSegments - 1; i++) {

        const UrlSegment& segmentToSearchFor = routeSegments[i];
        bool nextSegmentAlreadyExists = false;

        // Search for this segment that might be the child of the current node
        for (const std::shared_ptr<UrlSegment>& nextNode : currNode->next) {
            // If the segment already exists, just navigate to that
            if (nextNode->value == segmentToSearchFor.value) {
                prevNode = currNode;
                currNode = nextNode;
                nextSegmentAlreadyExists = true;
                break;
            }
        }

        // The segment does not exist yet, so create it
        if (nextSegmentAlreadyExists == false) {
            currNode->next.emplace_back(std::make_shared<UrlSegment>(
                segmentToSearchFor.value
            ));

            prevNode = currNode;
            currNode = currNode->next.back();
        }
    }

    /*
        `currNode` right now is pointing to the node that's going to be the parent
        of the node that we're interested in
        If it exists, just add the handler to the appropriate method
        If it does not, create a new node and add it to `currNode->next`
    */
    const std::string& segmentValueToAdd = routeSegments.back().value;

    bool segmentAlreadyExists = false;
    for (const std::shared_ptr<UrlSegment>& nextNode : currNode->next) {
        if (nextNode->value == segmentValueToAdd) {
            segmentAlreadyExists = true;
            nextNode->handlers.SetHandler(routeToAdd.method, handler);
            break;
        }
    }

    // If the segment doesn't exist yet, create a new one
    if (segmentAlreadyExists == false) {
        const std::shared_ptr<UrlSegment> newNode = std::make_shared<UrlSegment>(
            segmentValueToAdd
        );
        newNode->handlers.SetHandler(routeToAdd.method, handler);
        currNode->next.push_back(newNode);
    }

    return;
}


const UrlSegment* Router::FindSegmentForRoute(HttpRequest& req) const {

    const Route routeToFind(req.method, req.requestUrl);
    const std::vector<UrlSegment> segmentedRoute = BreakRouteIntoSegments(routeToFind.requestUrl);
    const size_t numSegments = segmentedRoute.size();

    // Handle case for root ("/") query
    if (numSegments == 1) {
        if (segmentedRoute[0].value == "/") {
            return m_dynamicRoutesTreeRoot.get();
        }
        return nullptr;
    }

    std::shared_ptr<UrlSegment> parent = m_dynamicRoutesTreeRoot;

    bool nextStaticNodeFound  = false;
    bool nextDynamicNodeFound = false;

    // `parent` will be pointing to the potential parent of whatever segment we're searching for
    for (size_t i = 1; i < numSegments; i++) {
        for (const std::shared_ptr<UrlSegment>& nextNode : parent->next) {
            if (nextNode->value == segmentedRoute[i].value) {
                nextStaticNodeFound = true;
                parent = nextNode;
                continue;
            }
        }

        if (nextStaticNodeFound == false) {
            for (const std::shared_ptr<UrlSegment>& nextNode : parent->next) {
                if (nextNode->isDynamic()) {
                    nextDynamicNodeFound = true;
                    parent = nextNode;

                    std::string routeParameterKey = nextNode->value.substr(1);
                    routeParameterKey.pop_back();

                    // Substitute route parameter here
                    req.routeParams.insert(std::make_pair(
                        routeParameterKey,
                        segmentedRoute[i].value
                    ));

                    continue;
                }
            }
        }

        if (nextStaticNodeFound == false && nextDynamicNodeFound == false) {
            return nullptr;
        }

        nextStaticNodeFound = false;
        nextDynamicNodeFound = false;
    }

    return parent.get();
}


/*
    @brief Get the handler function for the given route
    @param req HttpRequest object

    @return An optional object for the function
*/
const SegmentHandlerFunctions* Router::FetchFunctionsForRoute(
    HttpRequest& req
) const {

    SanitizeURL(req.requestUrl);

    // Look in static routes
    const auto it = m_staticRoutes.find(req.requestUrl);
    if (it != m_staticRoutes.end()) {
        return &(it->second);
    }

    // Look in dynamic routes
    const UrlSegment* segment = FindSegmentForRoute(req);
    if (segment == nullptr) {
        return nullptr;
    }

    return &(segment->handlers);
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
