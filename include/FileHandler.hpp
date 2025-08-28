#pragma once

#include <string>

#include "HttpMessage.hpp"

namespace FileHandler {
    HttpResponse MakeHttpResponseFromFile(const std::string& fileName);
}