#pragma once

#include <string>

#include "HttpMessage.hpp"

namespace FileHandler {
    HttpResponse MakeHttpResponseFromFile(const std::string& fileName);
    void ReadFileIntoBody(const std::string& fileName, HttpResponse& res);
}