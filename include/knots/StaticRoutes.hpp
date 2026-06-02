#pragma once

#include <filesystem>

#include "knots/Router.hpp"

namespace StaticRoutes {
    /*
        @brief Add a file as the response of a GET request
        @param path Path of fie
        @param router Router to add to
        @param prefixToRemove The prefix to remove from `path` if it exists
        If left blank, path will remove itself as a prefix from the added routes

        Example use case of `prefixToRemove`:
        Added file: "./static/1/txt"

        In this case, routes will include the "./static" part
            - "./static/1.txt"

        You may not like this, or may want to trim the route till a certain point
        Hence, you can opt to cleanup the "./static" part, and the routes will end up as such:
            - "/1.txt"

        Note: If the "current directory" symbol ".", is present, it'll be automatically removed
        if its at the start of the path
    */
    void AddStaticFile(
        const std::filesystem::path& path,
        Router& router,
        const std::string& prefixToRemove = ""
    );

    /*
        @brief Add all files in this directory as the response of a GET request
        @param path Path of directory
        @param router Router to add to
        @param prefixToRemove The prefix to remove from each file `path` if it exists
        If left blank, path will remove itself as a prefix from the added routes

        Example use case of `prefixToRemove`:
        Added directory: "./static/"
        Files in directory:
            - ./static/1.txt
            - ./static/2.txt

        In this case, routes will include the "./static" part
            - "./static/1.txt"
            - "./static/2.txt"

        You may not like this, or may want to trim the route till a certain point
        Hence, you can opt to cleanup the "./static" part, and the routes will end up as such:
        to clean up routes as such:
            - "/1.txt"
            - "/2.txt"

        Note: If the "current directory" symbol ".", is present, it'll be automatically removed
        if its at the start of the path
    */
    void AddStaticDirectory(
        const std::filesystem::path& path,
        Router& router,
        std::string prefixToRemove = ""
    );
}