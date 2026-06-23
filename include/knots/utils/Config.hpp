#pragma once

enum class RequestLoggingVerbosity {
    NONE,
    BASIC,
    INCLUDE_IP,
    INCLUDE_TIME,
    FULL
};

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
    RequestLoggingVerbosity requestLoggingVerbosity;

    constexpr HttpServerConfiguration() :
        port(-1),
        maxConnections(-1),
        inputPollingIntevalMs(-1),
        requestLoggingVerbosity(RequestLoggingVerbosity::NONE)
    {};

    constexpr HttpServerConfiguration(
        const int port,
        const int maxConnections,
        const bool inputPollingIntevalMs,
        const RequestLoggingVerbosity requestLoggingVerbosity
    ) :
        port(port),
        maxConnections(maxConnections),
        inputPollingIntevalMs(inputPollingIntevalMs),
        requestLoggingVerbosity(requestLoggingVerbosity)
    {};
};
