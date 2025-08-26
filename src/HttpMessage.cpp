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
bool HttpResponse::IsValid() const noexcept {
    return version != HttpVersion::DEFAULT_INVALID;
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