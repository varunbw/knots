#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <map>

struct File {
    std::filesystem::path name;
    std::unique_ptr<std::string> contents;

    File(const std::filesystem::path& name, std::unique_ptr<std::string>& contents) :
        name(name),
        contents(std::move(contents))
    {};
    File(const std::string& name, std::unique_ptr<std::string>& contents) :
        name(name),
        contents(std::move(contents))
    {};
};

namespace FileHandler {

    std::map<std::string, File> files;

    /*
        @brief Get contents of the file, returned as a `std::string`
        @param alias Alias or path of the file
    
        @return Contents of the file in a `std::string`
    */
    std::string GetFileContents(const std::filesystem::path& path);

    /*
        @brief Get the required file, but don't store it's contents in the FileHandler cache
        @param path Path of the file to read
    
        @return Contents of the file in a `std::string`

        Useful for large, infrequently used files, or if you simply don't want to cache something
    */
    std::string GetFileContentsWithoutCaching(const std::filesystem::path& path);

    /*
        @brief Update the contents of a cached file
        @param alias Alias or path of the file
    
        @return `true` if updated successfully, `false` otherwise
    */
    bool UpdateFile(const std::filesystem::path& path);
}