#ifndef KNOTS_HTTPPARSER_HPP
#define KNOTS_HTTPPARSER_HPP

#include <sstream>

#include "HttpMessage.hpp"

namespace MessageHandler {
    // HttpRequest ParseHttpRequest(std::stringstream&);

    HttpResponse BuildHttpResponse(const int responseCode);
    std::string SerializeHttpResponse(HttpResponse& res);
}

#endif