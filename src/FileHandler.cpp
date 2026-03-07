#include <format>
#include <fstream>

#include <knots/FileHandler.hpp>
#include <knots/Utils.hpp>

namespace FileHandler {
    /*
        @brief Read a file from disk into memory for faster access times
        @param path Path of the file to read

        @return `true` if file was read and cached successfully, `false` otherwise
    */
    bool ReadFileIntoMemory(const std::filesystem::path& path);
}

bool FileHandler::ReadFileIntoMemory(
    const std::filesystem::path& path
) {
    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);

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
    inputStream.read(contents->data(), fileSize);

    if (contents->size() != fileSize) {
        return false;
    }

    files.insert(std::make_pair(
        path,
        File(path, contents)
    ));

    return true;
}


std::string FileHandler::GetFileContents(const std::filesystem::path& path) {

    std::map<std::string, File>::iterator it = files.find(path.string());
    if (it != files.end()) {
        return *(it->second.contents);
    }

    if (ReadFileIntoMemory(path) == false) {
        return std::string();
    }

    return *(files.at(path).contents);
}

std::string FileHandler::GetFileContentsWithoutCaching(const std::filesystem::path& path) {

    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);

    if (inputStream.is_open() == false) {
        Log::Error(std::format(
            "GetFileContentsWithoutCaching(): Could not open file {}",
            path
        ));
        
        return std::string();
    }

    const std::streampos fileSize = inputStream.tellg();
    inputStream.seekg(0);

    std::unique_ptr<std::string> contents = std::make_unique<std::string>();
    inputStream.read(contents->data(), fileSize);

    if (contents->size() != fileSize) {
        return std::string();
    }

    return *contents;
}

bool FileHandler::UpdateFile(const std::filesystem::path& path) {
    return ReadFileIntoMemory(path);
}