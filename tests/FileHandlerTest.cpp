#include <filesystem>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <sys/types.h>

#include "knots/Utils.hpp"
#include "knots/FileHandler.hpp"

class RandomFileGenerator {
public:
    const std::string m_fileName = "FileHandlerTestSuite_RandomData.txt";
    std::vector<char> m_randomData;
    
    RandomFileGenerator();
    ~RandomFileGenerator();
};


RandomFileGenerator::RandomFileGenerator() {

    std::ofstream outfile(m_fileName, std::ios::binary | std::ios::out);

    if (outfile.is_open() == false) {
        Log::Error(std::format(
            "GenerateRandomData(): Could not create file {}",
            m_fileName
        ));
    }

    // Generate a 1 MB file of junk
    std::mt19937_64 mt(10597836);
    constexpr int fileSize = 1024 * 1024;

    m_randomData.reserve(fileSize);
    
    for (int i = 0; i < fileSize; i++) {
        m_randomData.push_back(static_cast<char>(mt()));
    }

    outfile.write(
        reinterpret_cast<const char*>(m_randomData.data()),
        fileSize
    );
}

RandomFileGenerator::~RandomFileGenerator() {
    if (std::filesystem::remove(m_fileName) == false) {
        Log::Error(std::format(
            "~RandomFileGenerator(): Failed to delete {}",
            m_fileName
        ));
    }
}


TEST(FileHandlerTest, FileReadsSuccessfully) {

    RandomFileGenerator rfg;
    const std::optional<std::string> fileContents = FileHandler::GetFileContents(rfg.m_fileName);

    // Check that value exists
    EXPECT_TRUE(fileContents.has_value());

    // Check for size mismatch
    EXPECT_EQ(fileContents->size(), rfg.m_randomData.size());

    std::vector<char> vec(fileContents->begin(), fileContents->end());

    // Check that the file is read correctly
    EXPECT_EQ(rfg.m_randomData, vec);
}


TEST(FileHandlerTest, FileReadFails) {

    const std::filesystem::path nonExistentFile = "./non-existent-file";

    if (std::filesystem::exists(nonExistentFile)) {
        std::filesystem::remove(nonExistentFile);
    }
    
    const std::optional<std::string> fileContents = FileHandler::GetFileContents(nonExistentFile);

    // Check if value exists
    EXPECT_FALSE(fileContents.has_value());
}


TEST(FileHandlerTest, FileReadsSuccessfullyWithoutCaching) {

    RandomFileGenerator rfg;
    const std::optional<std::string> fileContents = 
        FileHandler::GetFileContentsWithoutCaching(rfg.m_fileName);

    // Check that value exists
    EXPECT_TRUE(fileContents.has_value());

    // Check for size mismatch
    EXPECT_EQ(fileContents->size(), rfg.m_randomData.size());

    std::vector<char> vec(fileContents->begin(), fileContents->end());

    // Check that the file is read correctly
    EXPECT_EQ(rfg.m_randomData, vec);
}


TEST(FileHandlerTest, FileContentsUpdate) {

    RandomFileGenerator rfg;
    const std::optional<std::string> fileContents = FileHandler::GetFileContents(rfg.m_fileName);
    EXPECT_TRUE(fileContents.has_value());

    // Update file contents
    std::ofstream outputStream(rfg.m_fileName, std::ios::out);
    EXPECT_TRUE(outputStream.is_open());

    outputStream.write("abcd", 4);
    outputStream.close();

    FileHandler::UpdateFile(rfg.m_fileName);

    const std::optional<std::string> updatedFileContents = 
        FileHandler::GetFileContents(rfg.m_fileName);
    EXPECT_TRUE(updatedFileContents.has_value());

    EXPECT_NE(fileContents.value(), updatedFileContents.value());
}