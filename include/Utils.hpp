#ifndef KNOTS_UTILS_HPP
#define KNOTS_UTILS_HPP

#include <string>

/*
    @brief Configuration object for the HTTP server
    @param port Port to listen on
    @param rootDirectory Root directory for the server
    @param maxConnections Maximum number of connections to accept
*/
struct HttpServerConfiguration {
    int port;
    std::string routesPath;
    int maxConnections;
};

/*
    @brief Parse the given configuration file
    @param filePath Path to the configuration file
    @param config The configuration object to fill with the parsed values
    @return void
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

#endif