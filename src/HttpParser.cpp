#include <fstream>
#include <iostream>
#include <map>

#include "HttpParser.hpp"
#include "HttpMessage.hpp"
#include "Utils.hpp"


/*
    @brief Parses the HTTP Method in the start line of a HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if method could be parsed successfully, `false` otherwise
*/
bool ParseStartLineMethod(std::stringstream& ss, HttpRequest& req) {
    
    std::string buffer;
    ss >> buffer;

    // Cleaner than writing if-else's
    // ToDo: Profile this against if-else statements
    static const std::map<std::string, HttpMethod> methodMap = {
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
    @brief Parses the HTTP Version in the start line of a HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if version could be parsed successfully, `false` otherwise
*/
bool ParseStartLineHttpVersion(std::stringstream& ss, HttpRequest& req) {

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

    // Extract method
    if (ParseStartLineMethod(ss, req) == false)
        return false;

    // Extract URL
    // ToDo: See where this can be improved
    ss >> req.requestUrl;

    // Extract version
    if (ParseStartLineHttpVersion(ss, req) == false)
        return false;

   return true; 
}


/*
    @brief Parses the headers of an HTTP request
    @param ss The stringstream containing the raw request
    @param req The HttpRequest structure

    @return void
*/
void ParseHeaders(std::stringstream& ss, HttpRequest& req) {

    std::string line;
    while (std::getline(ss, line, '\n')) {
        // Remove any trailing \r if present
        if (line.size() && line.back() == '\r') {
            line.pop_back();
        }

        // An empty line marks the end of headers
        if (line.empty()) {
            return;
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

        req.headers[name] = value;

        // Log::Info(std::format(
        //     "ParseHeaders(): Added header {}: {}",
        //     line.substr(0, colonPos), req.headers[line.substr(0, colonPos)]
        // ));
    }

    return;
}


/*
    @brief Parse the HttpRequest message
*/
HttpRequest HttpParser::ParseHttpRequest(std::stringstream& ss) {

    HttpRequest req;
    if (ParseStartLine(ss, req) == false) {
        Log::Error("ParseHttpRequest(): Error while parsing previous request's start line");
        return {};
    }

    ParseHeaders(ss, req);

    req.PrintMessage();

    return req;
}