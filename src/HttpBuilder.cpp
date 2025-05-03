#include <map>

#include "MessageHandler.hpp"

namespace HttpResponseCodes {
    const std::map<int, std::string> statusText = {
        {100, "Continue"},
        {101, "Switching Protocols"},
        // {102, "Processing"}, // Deprecated
        {103, "Early Hints"},

        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"}, // WebDAV
        {208, "Already Reported"}, // WebDAV
        {226, "IM Used"},
        
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        // {305, "Use Proxy"}, // Deprecated
        // {306, ""}, // Unused
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},

        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"}, // lmao
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"}, // WebDAV
        {423, "Locked"}, // WebDAV
        {424, "Failed Dependency"}, // WebDAV
        {425, "Too Early"}, // Experimental
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},

        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"}, // WebDAV
        {508, "Loop Detected"}, // WebDAV
        {510, "Not Extended"},
        {511, "Network Authentication Required"}
    };
}

/*
    @brief Build an HttpResponse structure based on given parameters
    @param responseCode The HTTP Response Code to use (ex: 200, 404, etc)
    @param responseBody The body of the HTTP response

    @return An `HttpResponse` structure

    @note Recommended use of this function is
    `BuildHttpResponse(code, std::move(body))`, to utilize move semantics.
    This method does give up ownership of the contents of `body` however, use this only when
    you no longer need them.
*/
HttpResponse MessageHandler::BuildHttpResponse(const int responseCode, std::string responseBody) {

    HttpResponse res;

    // Start Line
    res.version = HttpVersion::HTTP_1_1;
    res.statusCode = responseCode;
    
    auto it = HttpResponseCodes::statusText.find(responseCode);
    if (it != HttpResponseCodes::statusText.end()) {
        res.statusText = it->second;
    }
    else {
        res.statusText = "Invalid HTTP Response Status Code";
    }
    
    // Headers
    // Only limited headers added for now
    res.headers["Content-Type"] = "text/html";
    res.headers["Content-Length"] = std::to_string(responseBody.size());
    res.headers["Connection"] = "close";
    res.headers["Server"] = "knots/0.1";

    // Body
    res.body = std::move(responseBody);
    
    return res;
}


/*
    @brief Serialize `res` into a std::string, with the standard HTTP response format
    @param `res` The HttpResponse structure

    @return The serialized response

    @note `res.body` will be empty after this operation, since `std::move()` is on it.
    Keep a copy beforehand if you need it
*/
std::string MessageHandler::SerializeHttpResponse(HttpResponse& res) {

    std::string resStr;
    
    size_t estimatedSize = 32 // Start line
        + res.headers.size() * 32 // Headers
        + 4 // CRLFCRLF
        + res.body.size(); // Body

    resStr.reserve(estimatedSize);

    // Start line
    resStr = std::format(
        "{} {} {}\r\n",
        res.version, res.statusCode, res.statusText
    );

    // Headers
    for (const auto& [key, value] : res.headers) {
        resStr += std::format(
            "{}: {}\r\n",
            key, value
        );
    }

    // Body
    resStr += "\r\n";
    resStr += std::move(res.body);

    return resStr;
}