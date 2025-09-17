#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "HttpMessage.hpp"
#include "Utils.hpp"


// -- Helper functions start

/*
    @brief Parses the HTTP Method in the start line of a HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if method could be parsed successfully, `false` otherwise
*/
bool ParseHttpMethod(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseHttpMethod(): Bad stream state"
        );
        return false;
    }

    std::string buffer;
    ss >> buffer;

    // Cleaner than writing if-else's
    // ToDo: Profile this against if-else statements
    static const std::map<std::string_view, HttpMethod> methodMap = {
        {"POST", HttpMethod::POST},
        {"GET", HttpMethod::GET},
        {"HEAD", HttpMethod::HEAD},
        {"PUT", HttpMethod::PUT},
        {"DELETE", HttpMethod::DELETE},
        {"CONNECT", HttpMethod::CONNECT},
        {"OPTIONS", HttpMethod::OPTIONS},
        {"TRACE", HttpMethod::TRACE},
        {"PATCH", HttpMethod::PATCH}
    };

    auto it = methodMap.find(buffer);
    if (it == methodMap.end()) {
        req.method = HttpMethod::DEFAULT_INVALID;
    }
    else {
        req.method = it->second;
    }

    return req.method != HttpMethod::DEFAULT_INVALID;
}

/*
    @brief Parses the Request URL and any parameters from the start line
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if method could be parsed successfully, `false` otherwise
*/
bool ParseUrlAndParameters(std::stringstream& ss, HttpRequest& req) {

    std::string urlAndParamsString;
    ss >> urlAndParamsString;

    // Request URL
    const std::size_t questionPos = urlAndParamsString.find('?');

    // URL Parsing
    // No params or trailing '?'
    if (questionPos == std::string::npos) {
        req.requestUrl = urlAndParamsString;
        return true;
    }

    // Basic parse
    req.requestUrl = urlAndParamsString.substr(0, questionPos);

    // Traling '?', no params, end here
    if (questionPos == urlAndParamsString.size() - 1) {
        return true;
    }

    // Parameter parsing
    // `left` and `right` are the left and right bounds of param respectively
    std::size_t left  = questionPos + 1;
    while (left < urlAndParamsString.size()) {
        std::size_t right = urlAndParamsString.find('&', left);
        if (right == std::string::npos) {
            right = urlAndParamsString.size();
        }

        // Move `right` to correctly be at the right bound of param from its
        // current position of either being at '&', or 1 beyond last index
        right--;
        
        std::string param = urlAndParamsString.substr(left, right - left + 1);
        std::size_t equalPos = param.find('=');

        Log::Info(std::format(
            "param: `{}`",
            param
        ));

        if (equalPos == std::string::npos) {
            if (param.size()) {
                req.params[param] = "";
            }
        }
        else {
            req.params[param.substr(0, equalPos)] = param.substr(equalPos + 1);
        }

        // Skip over '&' and go to next param's start
        left = right + 2;
    }

    return true;
}


/*
    @brief Parses the HTTP Version in the start line of a HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if version could be parsed successfully, `false` otherwise
*/
bool ParseHttpVersion(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseHttpVersion(): Bad stream state"
        );
        return false;
    }

    std::string buffer;
    std::getline(ss, buffer, '\n');

    if (buffer.back() == '\r')
        buffer.pop_back();

    // There is a space before 'HTTP' below because the stringstream `ss` is
    // reading an additional space into buffer; ex: " HTTP 1.0"
    // This is more efficient than performing an erase operation on the buffer
    static const std::map<std::string, HttpVersion> versionMap = {
        {" HTTP/1.0", HttpVersion::HTTP_1_0},
        {" HTTP/1.1", HttpVersion::HTTP_1_1},
        {" HTTP/2.0", HttpVersion::HTTP_2_0}
    };

    auto it = versionMap.find(buffer);
    if (it == versionMap.end()) {
        req.version = HttpVersion::DEFAULT_INVALID;
    }
    else {
        req.version = it->second;
    }

    return req.version != HttpVersion::DEFAULT_INVALID;
}

/*
    @brief Parse the start line of an HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if start line could be parsed sucessfully, `false` otherwise
*/
bool ParseStartLine(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseStartLine(): Bad stream state"
        );
        return false;
    }

    // Extract method
    if (ParseHttpMethod(ss, req) == false) {
        Log::Error(std::format(
            "failed here on ParseHttpMethod"
        ));
        return false;
    }

    // Extract URL and parameters
    if (ParseUrlAndParameters(ss, req) == false) {
        Log::Error(std::format(
            "failed here on ParseUrlAndParameters"
        ));
        return false;
    }

    // Extract version
    if (ParseHttpVersion(ss, req) == false) {
        Log::Error(std::format(
            "failed here on ParseHttpVersion"
        ));
        return false;
    }

   return true; 
}


/*
    @brief Parses the headers of an HTTP request
    @param ss Message in stringstream format
    @param req The HttpRequest structure
*/
bool ParseHeaders(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseHeaders(): Bad stream state"
        );
        return false;
    }

    std::string line;
    while (std::getline(ss, line, '\n')) {
        // Remove any trailing \r if present
        if (line.size() && line.back() == '\r') {
            line.pop_back();
        }

        // An empty line marks the end of headers
        if (line.empty()) {
            return true;
        }

        size_t colonPos = line.find(':');
        if (colonPos == line.npos) {
            Log::Error(std::format(
                "ParseHeaders(): Invalid header {}",
                line
            ));
            continue;
        }

        const std::string name  = line.substr(0, colonPos);
        const std::string value = line.substr(colonPos + 2);

        // HTTP Headers are case-insensitive
        req.headers[name] = value;
    }

    return false;
}


/*
    @brief Parse the body of an HTTP Request
    @param ss Message in stringstream format
    @param req The HttpRequest structure
*/
bool ParseBody(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseBody(): Bad stream state"
        );
        return false;
    }

    auto it = req.headers.find("content-length");
    if (it == req.headers.end()) {
        return true;
    }

    size_t contentLength = std::stoul(it->second);
    std::string body(contentLength, 0);

    // Read contentLength bytes of data from the stream to body
    ss.read(body.data(), contentLength);

    // gcount() returns the number of characters extracted by the previous unformatted input
    // function dispatched for this stream.
    // Accept the body only if this count matches expected length of `contentLength`
    if (ss.gcount() == static_cast<std::streamsize>(contentLength)) {
        req.body = std::move(body);
        return true;
    }

    Log::Error(std::format(
        "ParseBody(): Incomplete body read, got {} bytes, expected {} from header",
        ss.gcount(),
        contentLength
    ));

    return false;
}

// -- Helper functions end

// -- HttpRequest functions start

/*
    @brief Print a formatted HTTP Response to std::cout
*/
void HttpRequest::PrintMessage() const {

    std::cout << std::format(
        "\n------- HTTP Request -------\n"
        "  [METHOD] : {}\n"
        "  [URL]    : {}\n"
        "  [VERSION]: {}\n\n",
        this->method, this->requestUrl, this->version
    );

    std::cout << "HEADERS\n";
    for (auto& [key, value] : headers) {
        // ToDo: Remove this
        if (key == "Cookie")
            std::cout << std::format(
                "  {}: {}....\n", 
                key, std::string_view(value.begin(), value.begin() + 16)
            );
        else
            std::cout << std::format("  {}: {}\n", key, value);
    }

    std::cout << "\nPARAMETERS\n";
    for (const auto& [param, value] : this->params) {
        std::cout << std::format(
            "  {}: {}\n",
            param, value
        );
    }

    std::cout << '\n'
        << "BODY\n" << this->body << '\n'
        << "------- End Request -------\n\n";

    return;
}

/*
    @brief Parse the HttpRequest message
    @param ss Message in stringstream format

    @return The request in a `HttpRequest` struct
*/
bool HttpRequest::ParseFrom(std::stringstream& ss) {

    if (ss.good() == false) {
        Log::Error(
            "HttpRequest::ParseFrom(): Bad stream state, discarding previous request"
        );
        return false;
    }

    if (ParseStartLine(ss, *this) == false) {
        Log::Error(
            "HttpRequest::ParseFrom(): Could not parse start line, discarding previous request"
        );
        return false;
    }

    if (ParseHeaders(ss, *this) == false) {
        Log::Error(
            "HttpRequest::ParseFrom(): Could not parse headers, discarding previous request"
        );
        return false;
    }

    if (ParseBody(ss, *this) == false) {
        Log::Error(
            "HttpRequest::ParseFrom(): Could not parse body, discarding previous request"
        );
        return false;
    }

    this->PrintMessage();

    ss.str("");

    return true;
}

// -- HttpRequest functions end