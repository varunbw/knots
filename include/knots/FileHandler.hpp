#pragma once

#include <string>

#include "knots/HttpMessage.hpp"

namespace FileHandler {
    HttpResponse MakeHttpResponseFromFile(const std::string& fileName);
    void ReadFileIntoBody(const std::string& fileName, HttpResponse& res);
}