#ifndef KNOTS_HTTPPARSER_HPP
#define KNOTS_HTTPPARSER_HPP

#include <sstream>

#include "HttpMessage.hpp"

namespace MessageHandler {
    HttpResponse BuildHttpResponse(const int responseCode);
}

#endif