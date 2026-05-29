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

        Hence, this function will automatically remove "./static",
        to clean up routes as such:
            - "/1.txt"

        Alternatively, you can pass a custom prefix, ex: '.' to be removed
        Now, the routes will be
            - "/static/1.txt"
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

        Hence, this function will automatically remove "./static",
        to clean up routes as such:
            - "/1.txt"
            - "/2.txt"

        Alternatively, you can pass a custom prefix, ex: '.' to be removed
        Now, the routes will be
            - "/static/1.txt"
            - "/static/2.txt"
    */
    void AddStaticDirectory(
        const std::filesystem::path& path,
        Router& router,
        std::string prefixToRemove = ""
    );
}