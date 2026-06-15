#pragma once

#include <string>

/*
    @brief Configuration object for the HTTP server
    @param port Port to listen on
    @param rootDirectory Root directory for the server
    @param maxConnections Maximum number of connections to accept
*/
struct HttpServerConfiguration {
    int port;
    int maxConnections;
    int inputPollingIntevalMs;

    constexpr HttpServerConfiguration() :
        port(-1),
        maxConnections(-1),
        inputPollingIntevalMs(-1)
    {};

    constexpr HttpServerConfiguration(
        const int port,
        const int maxConnections,
        const bool inputPollingIntevalMs
    ) :
        port(port),
        maxConnections(maxConnections),
        inputPollingIntevalMs(inputPollingIntevalMs)
    {};
};


namespace Log {
    void Error(const std::string& message);
    void Warning(const std::string& message);
    void Success(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
}


/*
    @brief Make a formatted error message
    @param message The message to format
    @return The formatted error message

    This formats `message` in the following way:
    "[ERROR]: <message>" 
    If printed to stdout, it will be colored red
*/
std::string MakeErrorMessage(const std::string& message);
