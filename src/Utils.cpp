#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <yaml-cpp/yaml.h>

#include "Utils.hpp"

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
    @return void
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
    config.routesPath = yaml["routes-path"].as<std::string>();
    config.maxConnections = yaml["max-connections"].as<int>();

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