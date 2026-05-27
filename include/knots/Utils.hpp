#pragma once

#include <filesystem>
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

/*
    @brief Parse the given configuration file
    @param filePath Path to the configuration file
    @param config The configuration object to fill with the parsed values
*/
HttpServerConfiguration ParseConfigurationFile(const std::string& filePath);


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


// Forward declaration so I don't have to include `Router.hpp` here
class Router;

namespace StaticRoutes {
    void AddStaticFile(const std::filesystem::path& path, Router& router);
    void AddStaticDirectory(const std::filesystem::path& path, Router& router);
}