#include <format>
#include <fstream>
#include <iostream>

#include "../include/Utils.hpp"

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
void ParseConfigurationFile(const std::string& filePath, HttpServerConfiguration& config) {

    std::ifstream configFile(filePath);
    if (configFile.is_open() == false) {
        throw std::runtime_error(MakeErrorMessage(
            std::format("ParseConfigurationFile(): Failed to open {}", filePath)
        ));
    }

    std::string line;
    while (getline(configFile, line)) {
        try {
            if (line.find("port") != std::string::npos) {
                config.port = stoi(line.substr(line.find("=") + 1));
            }
            else if (line.find("root-directory") != std::string::npos) {
                // ToDo Sanitize input
                config.rootDirectory = line.substr(line.find("=") + 1);

                // Remove leading and trailing `"`
                config.rootDirectory.erase(config.rootDirectory.begin());
                config.rootDirectory.pop_back();
            }
            else if (line.find("max-connections") != std::string::npos) {
                config.maxConnections = stoi(line.substr(line.find("=") + 1));
            }
        }
        catch (const std::invalid_argument& e) {
            throw std::invalid_argument(MakeErrorMessage(
                std::format("ParseConfigurationFile(): Failed to parse parameter: {}", line)
            ));
        }
    }

    configFile.close();
    return;
}



namespace Log {
    void Error(const std::string& message) {
        std::cerr << std::format(
            RED_START "[ERROR]: {}\n" RESET_COLOR,
            message
        );
    }

    void Warning(const std::string& message) {
        std::cerr << std::format(
            YELLOW_START "[WARNING]: {}\n" RESET_COLOR,
            message
        );
    }

    void Success(const std::string& message) {
        std::cerr << std::format(
            GREEN_START "[SUCCESS]: {}\n" RESET_COLOR,
            message
        );
    }

    void Info(const std::string& message) {
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