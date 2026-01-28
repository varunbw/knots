#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <yaml-cpp/yaml.h>

#include "knots/Utils.hpp"

#include <thread>

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

std::string MakeErrorMessage(const std::string& message) {
    return std::format(
        RED_START "[ERROR]: {}\n" RESET_COLOR,
        message
    );
}