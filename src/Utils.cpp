#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <sys/socket.h>
#include <yaml-cpp/yaml.h>

#include "knots/FileHandler.hpp"
#include "knots/HttpMessage.hpp"
#include "knots/Router.hpp"
#include "knots/Utils.hpp"

/*
    Used for coloring the output text
    These are ANSI escape codes, see https://en.wikipedia.org/wiki/ANSI_escape_code
*/
#define RED_START "\033[1;31m"
#define GREEN_START "\033[1;32m"
#define YELLOW_START "\033[1;33m"
#define RESET_COLOR "\033[0m"


/*
    @breif Parse the given configuration file
    @param filePath Path to the configuration file
    @param config The configuration object to fill with the parsed values
*/
HttpServerConfiguration ParseConfigurationFile(const std::string& filePath) {

    YAML::Node yaml;
    HttpServerConfiguration config;

    try {
        yaml = YAML::LoadFile(filePath);
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
            filePath
        )));
    }
    catch (YAML::Exception& e) {
        throw std::runtime_error(MakeErrorMessage(std::format(
            "Unknown error while parsing configuration file: {}",
            filePath
        )));
    }

    config.port = yaml["port"].as<int>();
    config.maxConnections = yaml["max-connections"].as<int>();
    config.inputPollingIntevalMs = yaml["input-polling-interval-ms"].as<int>();

    return config;
}



namespace Log {

    std::mutex coutMutex;        

    void Error(const std::string& message) {
        std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
        std::cerr << std::format(
            RED_START "[ERROR]: {}\n" RESET_COLOR,
            message
        );
    }

    void Warning(const std::string& message) {
        std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
        std::cerr << std::format(
            YELLOW_START "[WARNING]: {}\n" RESET_COLOR,
            message
        );
    }

    void Success(const std::string& message) {
        std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
        std::cerr << std::format(
            GREEN_START "[SUCCESS]: {}\n" RESET_COLOR,
            message
        );
    }

    void Info(const std::string& message) {
        std::scoped_lock<std::mutex> coutMutexLock(coutMutex);
        std::cerr << std::format(
            "[INFO]: {}\n",
            message
        );
    }

    void Debug(const std::string& message) {
        std::cerr << std::format(
            "[DEBUG]: {}\n",
            message
        );
    }
}


std::string MakeErrorMessage(const std::string& message) {
    return std::format(
        RED_START "[ERROR]: {}\n" RESET_COLOR,
        message
    );
}


namespace fs = std::filesystem;

void StaticRoutes::AddStaticFile(const fs::path& path, Router& router) {

    if (path.empty()) {
        Log::Error(std::format(
            "StaticRoutes::AddStaticFile(): `{}` is an empty path",
            path.string()
        ));
        return;
    }

    router.Get(path.string(), 
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