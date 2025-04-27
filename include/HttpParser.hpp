#ifndef KNOTS_HTTPPARSER_HPP
#define KNOTS_HTTPPARSER_HPP

#include <sstream>

#include "HttpMessage.hpp"

namespace HttpParser {
    HttpRequest ParseHttpRequest(std::stringstream&);
}

#endif