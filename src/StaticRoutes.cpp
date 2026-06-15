#include <format>

#include "knots/FileHandler.hpp" 
#include "knots/HttpMessage.hpp"
#include "knots/StaticRoutes.hpp"
#include "knots/Utils.hpp"

namespace fs = std::filesystem;

void StaticRoutes::AddStaticFile(
    const fs::path& path,
    Router& router,
    const std::string& prefixToRemove
) {

    if (path.empty()) {
        Log::Error(std::format(
            "StaticRoutes::AddStaticFile(): `{}` is an empty path",
            path.string()
        ));
        return;
    }

    // Remove prefix (if provided and existing)
    const std::string route = [&path, &prefixToRemove] () {

        std::string buffer = path.string();
        
        if (buffer.starts_with(prefixToRemove)) {
            buffer.erase(0, prefixToRemove.size());
        }

        // Erase leading period "." if it exists
        if (buffer[0] == '.') {
            buffer.erase(buffer.begin());
        }

        // Add leading slash if it doesn't exist
        // This can be absent in cases where `prefixToRemove` contains a trailing '/'
        if (buffer[0] != '/') {
            buffer = "/" + buffer;
        }

        return buffer;
    } ();

    if (route.empty()) {
        Log::Warning(std::format(
            "Route evauluates to empty: `{}`, after removal of prefix `{}`",
            path.string(), prefixToRemove
        ));
        return;
    }

    // Simple GET request
    router.Get(route, 
        [path] (const HttpRequest& req, HttpResponse& res) {

            const std::optional<std::string> fileContents = FileHandler::GetFileContents(path);

            if (path.extension() == ".js") {
                res.SetHeader("Content-Type", "text/javascript");
            }

            if (fileContents.has_value()) {
                res.SetBody(fileContents.value());
                res.SetStatus(200);
                return;
            }

            res.SetStatus(404);
            return;
        }
    );

    return;
}



void StaticRoutes::AddStaticDirectory(
    const fs::path& path,
    Router& router,
    std::string prefixToRemove
) {

    if (fs::is_directory(path) == false) {
        Log::Error(std::format(
            "StaticRoutes::AddStaticDirectory(): `{}` is not a directory",
            path.string()
        ));
        return;
    }

    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            StaticRoutes::AddStaticFile(entry.path(), router, prefixToRemove);
        }
    }
    
    return;    
}