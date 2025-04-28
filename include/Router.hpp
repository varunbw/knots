#ifndef KNOTS_ROUTER_HPP
#define KNOTS_ROUTER_HPP

#include <map>
#include <string>

namespace Router {
    /*
        @brief Get the file path associated with the requested URL
        @param `requestUrl` The URL requested by the client
        @return The file path if a mapping is found, or an empty string if it isn't
    */
    std::string GetFilePath(const std::string& requestUrl) {

        static const std::map<std::string, std::string> routes = {
            {"/", "index.html"},
            {"/index.html", "index.html"},
            {"/test.html", "test.html"},
            {"/favicon.ico", "knots-icon.jpg"}
        };

        auto it = routes.find(requestUrl);
        if (it != routes.end()) {
            return std::string("static/" + it->second);
        }

        else {
            return {};
        }
    }
}

#endif