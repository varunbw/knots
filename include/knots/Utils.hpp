#pragma once

#include <chrono>
#include <iostream>
#include <optional>
#include <ratio>
#include <string>

#include "knots/Logger.hpp"

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

/*
    @brief Make a formatted error message
    @param message The message to format
    @return The formatted error message

    This formats `message` in the following way:
    "[ERROR]: <message>" 
    If printed to stdout, it will be colored red
*/
std::string MakeErrorMessage(const std::string& message);


using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

class Timer {
private:
    TimePoint m_start;
    const std::string m_message;

public:
    Timer(const std::string& message) : 
        m_start(Clock::now()),
        m_message(message)
    {};

    ~Timer() {
        std::cout << std::format(
            "{} : {:.4}ms\n",
            m_message,
            std::chrono::duration<double>(Clock::now() - m_start).count() * 1000
        );
        return;
    }
};