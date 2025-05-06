#pragma once

#include <string>

#include "HttpMessage.hpp"

namespace FileHandler {
    HttpResponse MakeHttpResponseFromFile(const int statusCode, const std::string& fileName);
}