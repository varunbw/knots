#include <algorithm>
#include <iostream>
#include <format>

#include "HttpMessage.hpp"

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
    @brief A wrapper to std::unordered_map<>::find(), but converts the given header to lowercase
    to ensure case-insentivity
    @param header The header to check

    @return The iterator to the header, or header.end() if not found

    @note The return type is `std::unordered_map<std::string, std::string>::const_iterator`
*/
auto HttpRequest::FindHeader(std::string header) const {
    std::transform(header.begin(), header.end(), header.begin(), ::tolower);
    return headers.find(header);
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
    @brief Check if the request is valid
    @return true if the request is valid\\
    @return false if the request is invalid

    @note A request is valid if the method is not DEFAULT_INVALID
    Crazy, ik
*/
bool HttpResponse::IsValid() const noexcept {
    return version != HttpVersion::DEFAULT_INVALID;
}

/*
    @brief A wrapper to std::unordered_map<>::find(), but converts the given header to lowercase
    to ensure case-insentivity
    @param header The header to check

    @return The iterator to the header, or header.end() if not found

    @note The return type is `std::unordered_map<std::string, std::string>::const_iterator`
*/
auto HttpResponse::FindHeader(std::string header) const {
    std::transform(header.begin(), header.end(), header.begin(), ::tolower);
    return headers.find(header);
}


/*
    @brief Print a formatted HTTP Response to std::cout

    @return void
*/
void HttpResponse::PrintMessage() const {

    std::cout << std::format(
        "\n------- HTTP Response -------\n"
        "  [VERSION]     : {}\n"
        "  [STATUS CODE] : {}\n"
        "  [STATUS TEXT] : {}\n\n",
        version, statusCode, statusText
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
        << "------- End Response -------\n\n";
    
    return;
}