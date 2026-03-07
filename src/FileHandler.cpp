#include <format>
#include <fstream>

#include <knots/FileHandler.hpp>
#include <knots/Utils.hpp>

namespace FileHandler {
    /*
        @brief Read a file from disk into memory for faster access times
        @param path Path of the file to read
        @param alias An optional alias to give, if left blank, the path is used as the alias instead

        Ex:
        - ReadFileIntoMemory("~/project/index.html", "index.html");
            This will allow you to use "index.html" to get the file contents later on

        - ReadFileIntoMemory("~/project/index.html");
            This will force you to use the complete file path when getting the file contents later on

        @return `true` if file was read and cached successfully, `false` otherwise
    */
    bool ReadFileIntoMemory(const std::filesystem::path& path, const std::string& alias = "");
}

bool FileHandler::ReadFileIntoMemory(
    const std::filesystem::path& path,
    const std::string& alias
) {

    std::ifstream inputStream(path.string(), std::ios::binary | std::ios::ate);

    if (inputStream.is_open() == false) {
        Log::Error(std::format(
            "ReadFileIntoMemory(): Could not open file {}",
            path
        ));

        return false;
    }

    const std::streampos fileSize = inputStream.tellg();
    inputStream.seekg(0);

    std::unique_ptr<std::string> contents = std::make_unique<std::string>();
    inputStream.read(contents.get()->data(), fileSize);

    if (contents.get()->size() != fileSize) {
        return false;
    }

    File fileToAdd(
        alias.empty() ? path.string() : alias,
        contents        
    );

    files.insert(std::make_pair(
        fileToAdd.name,
        fileToAdd
    ));
}