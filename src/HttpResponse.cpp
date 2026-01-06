#include <iostream>
#include <map>

#include "knots/HttpMessage.hpp"

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
    @brief Set status code and its corresponding text
    @param statusCode Code to set for this response object
*/
void HttpResponse::SetStatus(const int statusCode) {

    this->statusCode = statusCode;

    auto it = HttpResponseCodes::statusText.find(statusCode);
    if (it != HttpResponseCodes::statusText.end()) {
        this->statusText = it->second;
    }
    else {
        throw std::invalid_argument(std::format(
            "Invalid status code: {} in HttpResponse::SetStatus",
            statusCode
        ));
    }

    return;
}

void HttpResponse::SetHeader(const std::string& key, const std::string& value) {
    this->headers[key] = value;
    return;
}

std::optional<std::string> HttpResponse::GetHeader(const std::string& key) const {

    auto it = this->headers.find(key);
    if (it == this->headers.end()) {
        return std::nullopt;
    }

    return it->second;
}

void HttpResponse::DeleteHeader(const std::string& key) {
    this->headers.erase(key);
    return;
}


void HttpResponse::SetBody(const std::string& body, const bool setContentLengthHeader) {

    this->body = body;
    if (setContentLengthHeader) {
        this->SetHeader("Content-Length", std::to_string(this->body.size()));
    }

    return;
}

void HttpResponse::SetBody(std::string&& body, const bool setContentLengthHeader) {

    this->body = std::move(body);
    if (setContentLengthHeader) {
        this->SetHeader("Content-Length", std::to_string(this->body.size()));
    }

    return;
}


/*
    @brief Serialize `res` into a std::string, with the standard HTTP response format

    @return The serialized response
*/
std::string HttpResponse::Serialize() const {

    size_t totalHeadersSize = 0;
    for (const auto& header : this->headers) {
        // "Key: Value\r\n"
        // Key + Value + 4 (1 colon, 1 space, 1 \r, 1 \n)
        totalHeadersSize += header.first.size() + header.second.size() + 4;
    }

    std::string res;
    size_t estimatedResSize = 32 // Start line
        + totalHeadersSize // Headers
        + 2 // CRLF
        + this->body.size(); // Body
    res.reserve(estimatedResSize);

    // Start line
    res = std::format(
        "{} {} {}\r\n",
        this->version, this->statusCode, this->statusText
    );

    // Headers
    for (const auto& header : this->headers) {
        res += std::format(
            "{}: {}\r\n",
            header.first, header.second
        );
    }

    res += "\r\n";
    res += this->body;
    
    return res;
}



/*
    @brief Print a formatted HTTP Response to std::cout
*/
void HttpResponse::PrintMessage() const {

    std::cout << std::format(
        "\n------- HTTP Response -------\n"
        "  [VERSION]     : {}\n"
        "  [STATUS CODE] : {}\n"
        "  [STATUS TEXT] : {}\n\n",
        this->version, this->statusCode, this->statusText
    );

    std::cout << "HEADERS\n";
    for (const auto& [key, value] : this->headers) {
        // ToDo: Remove this
        if (key == "Cookie")
            std::cout << std::format(
                "  {}: {}....\n",
                key, std::string_view(value.begin(), value.begin() + 16)
            );
        else
            std::cout << std::format("  {}: {}\n", key, value);
    }

    std::cout << '\n'
        << "BODY\n" << this->body << '\n'
        << "------- End Response -------\n\n";

    return;
}