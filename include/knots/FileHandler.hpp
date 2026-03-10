#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>

struct File {
    std::filesystem::path path;
    std::unique_ptr<std::string> contents;

    File(const std::filesystem::path& path, std::unique_ptr<std::string>& contents) :
        path(path),
        contents(std::move(contents))
    {};
    File(const std::string& name, std::unique_ptr<std::string>& contents) :
        path(name),
        contents(std::move(contents))
    {};
};

class FileHandler {
private:
    inline static std::shared_mutex m_mutex;
    inline static std::map<std::string, File> m_files;

    /*
        @brief Read a file from disk into memory for faster access times
        @param path Path of the file to read

        @return `true` if file was read and cached successfully, `false` otherwise
    */
    static bool ReadFileIntoMemory(const std::filesystem::path& path);

public:
    /*
        @brief Get contents of the file, returned as a `std::string`
        @param alias Alias or path of the file
    
        @return Contents of the file in a `std::string`
    */
    static std::string GetFileContents(const std::filesystem::path& path);

    /*
        @brief Get the required file, but don't store it's contents in the FileHandler cache
        @param path Path of the file to read
    
        @return Contents of the file in a `std::string`

        Useful for large, infrequently used files, or if you simply don't want to cache something
    */
    static std::string GetFileContentsWithoutCaching(const std::filesystem::path& path);

    /*
        @brief Update the contents of a cached file
        @param alias Alias or path of the file
    
        @return `true` if updated successfully, `false` otherwise
    */
    static bool UpdateFile(const std::filesystem::path& path);
};