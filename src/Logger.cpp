#include <queue>

#include "knots/Utils.hpp"

std::jthread loggerThread;
std::atomic<bool> isLoggerRunning;

std::mutex queueMutex;
std::queue<std::string> logQueue;

bool Logger::IsLoggerRunning() {
    return isLoggerRunning;
}

void Logger::StartLogger() {

    isLoggerRunning = true;

    loggerThread = std::jthread(
        [] () {
            while (isLoggerRunning) {
                std::scoped_lock<std::mutex> lock(queueMutex);
                if (logQueue.size()) {
                    std::cout << logQueue.front();
                    logQueue.pop();
                }
            }
        }
    );
}

void Logger::StopLogger() {
    isLoggerRunning = false;
}

void Logger::EnqueueLog(const std::string& message) {
    std::scoped_lock<std::mutex> lock(queueMutex);
    logQueue.emplace(message);
    return;
}

void Logger::Error(const std::string& message) {
    Timer timer("Error");
    // std::scoped_lock<std::mutex> lock(queueMutex);
    Logger::EnqueueLog(std::format(
        RED_START "[ERROR]: {}\n" RESET_COLOR,
        message
    ));
}

void Logger::Warning(const std::string& message) {
    Timer timer("Warning");
    // std::scoped_lock<std::mutex> lock(queueMutex);
    Logger::EnqueueLog(std::format(
        YELLOW_START "[WARNING]: {}\n" RESET_COLOR,
        message
    ));
}

void Logger::Success(const std::string& message) {
    Timer timer("Success");
    // std::scoped_lock<std::mutex> lock(queueMutex);
    Logger::EnqueueLog(std::format(
        GREEN_START "[SUCCESS]: {}\n" RESET_COLOR,
        message
    ));
}

void Logger::Info(const std::string& message) {
    Timer timer("Info");
    // std::scoped_lock<std::mutex> lock(queueMutex);
    Logger::EnqueueLog(std::format(
        "[INFO]: {}\n",
        message
    ));
}

void Logger::Debug(const std::string& message) {
    Timer timer("Debug");
    // std::scoped_lock<std::mutex> lock(queueMutex);
    Logger::EnqueueLog(std::format(
        "[DEBUG]: {}\n",
        message
    ));
}
// void Logger::Error(const std::string& message) {
//     Timer timer("Error");
    // std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
//     std::cout << std::format(
//         RED_START "[ERROR]: {}\n" RESET_COLOR,
//         message
//     );
// }

// void Logger::Warning(const std::string& message) {
//     Timer timer("Warning");
    // std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
//     std::cout << std::format(
//         YELLOW_START "[WARNING]: {}\n" RESET_COLOR,
//         message
//     );
// }

// void Logger::Success(const std::string& message) {
//     Timer timer("Success");
    // std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
//     std::cout << std::format(
//         GREEN_START "[SUCCESS]: {}\n" RESET_COLOR,
//         message
//     );
// }

// void Logger::Info(const std::string& message) {
//     Timer timer("Info");
    // std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
//     std::cout << std::format(
//         "[INFO]: {}\n",
//         message
//     );
// }

// void Logger::Debug(const std::string& message) {
//     Timer timer("Debug");
    // std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
//     std::cout << std::format(
//         "[DEBUG]: {}\n",
//         message
//     );
// }
