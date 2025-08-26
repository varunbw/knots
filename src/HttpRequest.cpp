#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

#include "MessageHandler.hpp"
#include "HttpMessage.hpp"
#include "Utils.hpp"


// -- Helper functions start

/*
    @brief Parses the HTTP Method in the start line of a HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if method could be parsed successfully, `false` otherwise
*/
bool ParseStartLineMethod(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseStartLineMethod(): Bad stream state"
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
    @brief Parses the HTTP Version in the start line of a HTTP Request
    @param ss Message in stringstream format
    @param req HttpRequest structure to parse message into

    @return `true` if version could be parsed successfully, `false` otherwise
*/
bool ParseStartLineHttpVersion(std::stringstream& ss, HttpRequest& req) {

    if (ss.good() == false) {
        Log::Error(
            "ParseStartLineHttpVersion(): Bad stream state"
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
    @param ss Message in stringstream format
    @param req The HttpRequest structure

    @return void
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
        // std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        req.headers[name] = value;

        // Log::Info(std::format(
        //     "ParseHeaders(): Added header {}: {}",
        //     line.substr(0, colonPos), req.headers[line.substr(0, colonPos)]
        // ));
    }

    return false;
}


/*
    @brief Parse the body of an HTTP Request
    @param ss Message in stringstream format
    @param req The HttpRequest structure

    @return void
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
    @brief Check if the request is valid
    @return true if the request is valid\\
    @return false if the request is invalid

    @note A request is valid if the method is not DEFAULT_INVALID
    Crazy, ik
*/
bool HttpRequest::IsValid() const noexcept {
    return method != HttpMethod::DEFAULT_INVALID;
}


/*
    @brief Print a formatted HTTP Response to std::cout
    
    @return void
*/
void HttpRequest::PrintMessage() const {

    std::cout << std::format(
        "\n------- HTTP Request -------\n"
        "  [METHOD] : {}\n"
        "  [URL]    : {}\n"
        "  [VERSION]: {}\n\n",
        method, requestUrl, version
    );

    std::cout << "HEADERS\n";
    for (auto& [key, value] : headers) {
        // ToDo: Remove this
        if (key == "Cookie")
            std::cout << std::format("  {}: {}....\n", key, std::string_view(value.begin(), value.begin() + 16));
        else
        std::cout << std::format("  {}: {}\n", key, value);
    }
    
    std::cout << '\n'
        << "BODY\n" << body << '\n'
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

    // HttpRequest req;
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
    return true;
}

// -- HttpRequest functions end
