#pragma once

#include <filesystem>

// Forward declaration so I don't have to include `Router.hpp` here
class Router;

namespace StaticRoutes {
    void AddStaticFile(const std::filesystem::path& path, Router& router);
    void AddStaticDirectory(const std::filesystem::path& path, Router& router);
}