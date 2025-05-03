#ifndef KNOTS_HTTPPARSER_HPP
#define KNOTS_HTTPPARSER_HPP

#include <sstream>

#include "HttpMessage.hpp"

namespace MessageHandler {
    HttpRequest ParseHttpRequest(std::stringstream&);
    HttpResponse BuildHttpResponse(const std::string& response);
}

#endif