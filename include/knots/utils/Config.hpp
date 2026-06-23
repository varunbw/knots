#pragma once

#include <string_view>

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
    Configuration object for the HTTP server
    - port
        Port to listen on

    - rootDirectory
        Root directory for the server

    - maxConnections
        Maximum number of connections to accept

    - requestLoggingVerbosity
        Verbosity at which to log the incoming requests, and their response code

    - timeZone
        To provide correct time for logging, see
        https://en.cppreference.com/cpp/chrono/zoned_time
        https://en.wikipedia.org/wiki/List_of_tz_database_time_zones

        Check "TZ Identifier" column in the wikipedia link to get your timezone
*/
struct HttpServerConfiguration {
    int port;
    int maxConnections;
    int inputPollingIntevalMs;
    RequestLoggingVerbosity requestLoggingVerbosity;
    std::string_view timeZone;
};
