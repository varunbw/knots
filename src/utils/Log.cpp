#include <format>
#include <iostream>
#include <mutex>

#include "knots/utils/Log.hpp"

/*
    Used for coloring the output text
    These are ANSI escape codes, see https://en.wikipedia.org/wiki/ANSI_escape_code
*/
#define RED_START "\033[1;31m"
#define GREEN_START "\033[1;32m"
#define YELLOW_START "\033[1;33m"
#define RESET_COLOR "\033[0m"


namespace Log {

    std::mutex cerrMutex;        

    void Error(const std::string_view message) {
        std::scoped_lock<std::mutex> coutMutexLock(cerrMutex);
        std::cerr << std::format(
            RED_START "[ERROR]: {}\n" RESET_COLOR,
            message
        );
    }

    void Warning(const std::string_view message) {
        std::scoped_lock<std::mutex> coutMutexLock(cerrMutex);
        std::cerr << std::format(
            YELLOW_START "[WARNING]: {}\n" RESET_COLOR,
            message
        );
    }

    void Success(const std::string_view message) {
        std::scoped_lock<std::mutex> coutMutexLock(cerrMutex);
        std::cerr << std::format(
            GREEN_START "[SUCCESS]: {}\n" RESET_COLOR,
            message
        );
    }

    void Info(const std::string_view message) {
        std::scoped_lock<std::mutex> cerrMutexLock(cerrMutex);
        std::cerr << std::format(
            "[INFO]: {}\n",
            message
        );
    }

    void Debug(const std::string_view message) {
        std::scoped_lock<std::mutex> cerrMutexLock(cerrMutex);
        std::cerr << std::format(
            "[DEBUG]: {}\n",
            message
        );
    }

    void Raw(const std::string_view message, const std::string_view end) {
        std::scoped_lock<std::mutex> cerrMutexLock(cerrMutex);
        std::cerr << message << end;
    }

    std::string MakeErrorMessage(const std::string_view message) {
        return std::format(
            RED_START "[ERROR]: {}\n" RESET_COLOR,
            message
        );
    }
}


