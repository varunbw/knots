#pragma once

#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
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

public:
    /*
        @brief Read a file from disk into memory for faster access times
        @param path Path of the file to read

        @return `true` if file was read and cached successfully, `false` otherwise

        You can use this function beforehand before starting up the server so that
        all the files are cached in memory
    */
    static bool CacheFile(const std::filesystem::path& path);

    /*
        @brief Get contents of the file, returned as a `std::string`
        @param alias Alias or path of the file
    
        @return Contents of the file in a `std::string`

        @note You do not need to call FileHandler::CacheFile before calling this
        This function will cache the file if its not already been cached
    */
    static std::optional<std::string> GetFileContents(const std::filesystem::path& path);

    /*
        @brief Get the required file, but don't store it's contents in the FileHandler cache
        @param path Path of the file to read, bypassing cache
    
        @return Contents of the file in a `std::string`

        Useful for large, infrequently used files, or if you simply don't want to cache something
    */
    static std::optional<std::string> GetFileContentsWithoutCaching(const std::filesystem::path& path);

    /*
        @brief Update the contents of a cached file
        @param path Path of the file to update
    
        @return `true` if updated successfully, `false` otherwise
    */
    static bool UpdateFile(const std::filesystem::path& path);

    /*
        @brief Remove the requested file path from cache if it exists
        @param path Path of to file to remove
    
        @return `true` if deleted successfully, `false` otherwise
    */
    static bool RemoveFileFromCache(const std::filesystem::path& path);

    /*
        @brief Return the number of files stored in cache
    
        @return Number of files stored in cache!!!!
    */
    static size_t GetCacheSize();
};