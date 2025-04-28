#include <format>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "Router.hpp"
#include "Utils.hpp"

/*
    @brief Loads the routes specified in the configuration file into a std::map
    @param configFilePath Path to the configuration file

    @return void
*/
void Router::LoadRoutesFromConfig(const std::string& configFilePath) {

    YAML::Node config;

    try {
        config = YAML::LoadFile(configFilePath);
    }
    catch (YAML::ParserException& e) {
        throw std::runtime_error(MakeErrorMessage(std::format(
            "Failed to parse configuration file: {}",
            e.what()
        )));
    }
    catch (YAML::BadFile& e) {
        throw std::runtime_error(MakeErrorMessage(std::format(
            "Configuration file not found: {}",
            configFilePath
        )));
    }
    catch (YAML::Exception& e) {
        throw std::runtime_error(MakeErrorMessage(std::format(
            "Unknown error while parsing configuration file: {}",
            configFilePath
        )));
    }

    // Load all routes in config["routes"], store them in this->routes
    for (const auto& route : config["routes"]) {
        const std::string path = route.first.as<std::string>();
        routes[path] = {
            route.second["file"].as<std::string>(),
            route.second["type"].as<std::string>()
        };
    }

    return;
}


/*
    @brief Return the requested route
    @param requestUrl The request URL (damn)

    @return The route associated with the URL if found, empty Route struct if not
*/
Route Router::GetRoute(const std::string& requestUrl) const {

    auto it = routes.find(requestUrl);
    if (it != routes.end()) {
        return it->second;
    }

    // Return empty route if not found
    return Route();
}