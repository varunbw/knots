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

    /*
        @brief Get contents of the file, returned as a `std::string`
        @param alias Alias or path of the file
    
        @return Contents of the file in a `std::string`
    */
    std::string GetFile(const std::string& alias);

    /*
        @brief Get the required file, but don't store it's contents in the FileHandler cache
        @param path Path of the file to read
    
        @return Contents of the file in a `std::string`

        Useful for large, infrequently used files, or if you simply don't want to cache something
    */
    std::string GetFileWithoutCaching(const std::filesystem::path& path);

    /*
        @brief Update the contents of a cached file
        @param alias Alias or path of the file
    
        @return `true` if updated successfully, `false` otherwise
    */
    bool UpdateFile(const std::string& alias);
}