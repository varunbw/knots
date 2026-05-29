#include "knots/StaticRoutes.hpp"

namespace fs = std::filesystem;

void StaticRoutes::AddStaticFile(const fs::path& path, Router& router) {

    if (path.empty()) {
        Log::Error(std::format(
            "StaticRoutes::AddStaticFile(): `{}` is an empty path",
            path.string()
        ));
        return;
    }

    // const std::string route = [path] () {
    //     std::size_t counter = 0;
    //     while (path.string()[counter] == '.') {
    //         counter++;
    //     }

    //     return path.string().substr(counter);
    // } ();

    // if (route.empty()) {
    //     Log::Warning(std::format(
    //         "Route evauluates to empty: {}, after removal of unnecessary characters",
    //         path.string()
    //     ));
    //     return;
    // }

    router.Get(path.string().substr(1), 
        [path] (const HttpRequest& req, HttpResponse& res) {

            const std::optional<std::string> fileContents = FileHandler::GetFileContents(path);

            if (fileContents.has_value()) {
                res.SetBody(fileContents.value());
                res.SetStatus(200);
                return;
            }

            res.SetStatus(404);
            return;
        }
    );

    Log::Success(std::format(
        "Added {}",
        path.string()
    ));

    return;
}



void StaticRoutes::AddStaticDirectory(const fs::path &path, Router &router) {

    if (fs::is_directory(path) == false) {
        Log::Error(std::format(
            "StaticRoutes::AddStaticDirectory(): `{}` is not a directory",
            path.string()
        ));
        return;
    }

    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            StaticRoutes::AddStaticFile(entry.path(), router);
        }
    }
    
    return;    
}