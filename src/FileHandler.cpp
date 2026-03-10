#include <format>
#include <fstream>
#include <mutex>
#include <shared_mutex>

#include <knots/FileHandler.hpp>
#include <knots/Utils.hpp>

bool FileHandler::ReadFileIntoMemory(
    const std::filesystem::path& path
) {
    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);

    if (inputStream.is_open() == false) {
        Log::Error(std::format(
            "ReadFileIntoMemory(): Could not open file {}",
            path.string()
        ));

        return false;
    }

    const std::streampos fileSize = inputStream.tellg();
    inputStream.seekg(0);

    std::unique_ptr<std::string> contents = std::make_unique<std::string>();
    contents->resize(fileSize);
    inputStream.read(contents->data(), fileSize);

    if (static_cast<long>(contents->size()) != fileSize) {
        return false;
    }

    std::unique_lock writeLock(FileHandler::m_mutex);
    m_files.insert(std::make_pair(
        path,
        File(path, contents)
    ));

    return true;
}


std::string FileHandler::GetFileContents(const std::filesystem::path& path) {

    {
        std::shared_lock readLock(FileHandler::m_mutex);

        std::map<std::string, File>::iterator it = m_files.find(path.string());
        if (it != m_files.end()) {
            return *(it->second.contents);
        }
    }

    if (ReadFileIntoMemory(path) == false) {
        return std::string();
    }

    return *(m_files.at(path).contents);
}


std::string FileHandler::GetFileContentsWithoutCaching(const std::filesystem::path& path) {

    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);

    if (inputStream.is_open() == false) {
        Log::Error(std::format(
            "GetFileContentsWithoutCaching(): Could not open file {}",
            path.string()
        ));
        
        return std::string();
    }

    const std::streampos fileSize = inputStream.tellg();
    inputStream.seekg(0);

    std::unique_ptr<std::string> contents = std::make_unique<std::string>();
    inputStream.read(contents->data(), fileSize);

    if (static_cast<long>(contents->size()) != fileSize) {
        return std::string();
    }

    return *contents;
}


bool FileHandler::UpdateFile(const std::filesystem::path& path) {
    return ReadFileIntoMemory(path);
}