#pragma once

#include <string>

namespace Log {
    void Error(const std::string& message);
    void Warning(const std::string& message);
    void Success(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
    void Raw(const std::string& message, const std::string_view end = "\n");
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
