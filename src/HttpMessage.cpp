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
    @brief Check if the request has a header
    @param header The header to check
    @return true if the request has the header\\
    @return false if the request does not have the header

    @note The header is case insensitive
*/
bool HttpRequest::HasHeader(const std::string& header) const {
    return headers.contains(header);
}


void HttpRequest::PrintMessage() const {

    std::cout << std::format(
        "------- HTTP Message -------\n"
        "  [METHOD] : {}\n"
        "  [URL]    : {}\n"
        "  [VERSION]: {}\n\n",
        method, requestUrl, version
    );

    std::cout << "HEADERS\n";
    for (auto& [key, value] : headers) {
        std::cout << std::format("  {} : {}\n", key, value);
    }
    
    std::cout << '\n'
        << "BODY\n" << body << '\n'
        << "------- End Message -------\n";
    
    return;
}