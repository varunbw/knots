#pragma once

#include <mutex>
#include <string>
#include <thread>

/*
    Used for coloring the output text
    These are ANSI escape codes, see https://en.wikipedia.org/wiki/ANSI_escape_code
*/
#define RED_START "\033[1;31m"
#define GREEN_START "\033[1;32m"
#define YELLOW_START "\033[1;33m"
#define RESET_COLOR "\033[0m"

namespace Logger {

    void EnqueueLog(const std::string& message);

    bool IsLoggerRunning();
    void StartLogger();
    void StopLogger();

    void Error(const std::string& message);
    void Warning(const std::string& message);
    void Success(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
};
