#pragma once

#include <string>
#include <vector>

namespace NetworkIO {
    /*
        @brief Wrapper to `send()` function in Linux, providing some error handling boilerplate
        @param clientSocketFD Socket FD of the intended recipient
        @param buffer The data to send in `std::string` format
        @param flags Any flags to pass to `send()`
    
        @return `true` if message was sent successfully, `false` otherwise
    */
    bool Send(const int socketFD, const std::string& buffer, const int flags);

    /*
        @brief Wrapper to `send()` function in Linux, providing some error handling boilerplate
        @param clientSocketFD Socket FD of the intended recipient
        @param buffer The data to send in `std::vector<char>` format
        @param flags Any flags to pass to `send()`
    
        @return `true` if message was sent successfully, `false` otherwise
    */
    bool Send(const int socketFD, const std::vector<char>& buffer, const int flags);
}