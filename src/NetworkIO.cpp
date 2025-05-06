
#include <format>
#include <string.h>
#include <string_view>

#include <sys/socket.h>

#include "Utils.hpp"
#include "NetworkIO.hpp"

namespace NetworkIO {
    /*
        @brief Wrapper to `send()` function in Linux, providing some error handling boilerplate
        @param clientSocketFD Socket FD of the intended recipient
        @param buffer The data to send in `std::string` format
        @param flags Any flags to pass to `send()`
    
        @return `true` if message was sent successfully, `false` otherwise
    */
    bool Send(const int socketFD, const std::string& buffer, const int flags) {

        if (send(socketFD, buffer.data(), buffer.size(), flags) < 0) {
            // Determine whether to print either the first 5 bytes, or whatever the buffer is
            size_t maxSize = std::min(static_cast<size_t>(5), buffer.size());

            Log::Error(std::format(
                "NetworkIO::Send(): Error sending message {}... of size {} to socket {}\n",
                std::string_view(buffer.begin(), buffer.begin() + maxSize),
                buffer.size(),
                socketFD
            ));
            return false;
        }
        
        return true;
    }

    /*
        @brief Wrapper to `send()` function in Linux, providing some error handling boilerplate
        @param clientSocketFD Socket FD of the intended recipient
        @param buffer The data to send in `std::vector<char>` format
        @param flags Any flags to pass to `send()`
    
        @return `true` if message was sent successfully, `false` otherwise
    */
    bool Send(const int socketFD, const std::vector<char>& buffer, const int flags) {

        if (send(socketFD, buffer.data(), buffer.size(), flags) < 0) {
            // Determine whether to print either the first 5 bytes, or whatever the buffer is
            size_t maxSize = std::min(static_cast<size_t>(5), buffer.size());

            Log::Error(std::format(
                "NetworkIO::Send(): Error sending message {}... of size {} to socket {} : {}\n",
                std::string_view(buffer.begin(), buffer.begin() + maxSize),
                buffer.size(),
                socketFD,
                strerror(errno)
            ));
            return false;
        }
        
        return true;
    }
}