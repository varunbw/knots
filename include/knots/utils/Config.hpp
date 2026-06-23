#pragma once

/*
    Specify the verbosity at which the server should log a req-res combo

    1. NONE
        Quite literally that

    2. BASIC
        Res Code, Req Method, "Req URL"
        ex: 200 GET "/index.html"

    3. INCLUDE_IP
        Add IP to BASIC
        ex: 127.0.0.1       - 200 GET "/index.html"
        The padding is meant to accomodate all IPs, so everything is in a neat line

    4. INCLUDE_TIME
        Add Date & Time info to BASIC
        ex: [2026-06-23 16:55:07.48] - 200 GET "/index.html"

    5. FULL
        Everything
        ex: [2026-06-23 16:54:25.24] 127.0.0.1       - 200 GET "/index.html"
*/
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
    @param requestLoggingVerbosity Verbosity at which to log the incoming requests, and their response code
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
