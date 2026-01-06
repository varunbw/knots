#include <bit>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <random>

#include "knots/Utils.hpp"
#include "knots/FileHandler.hpp"

class RandomFileGenerator {
public:
    const std::string m_fileName = "FileHandlerTestSuite_RandomData.txt";
    std::vector<uint64_t> m_randomData;
    
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
    
    for (int i = 0; i < fileSize; i += 8) {
        m_randomData.push_back(mt());
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

TEST(FileHandlerTest, ReadFromValidFile) {
    RandomFileGenerator fileGen;

    HttpResponse res =
        FileHandler::MakeHttpResponseFromFile(fileGen.m_fileName);

    const size_t resBodySize = res.body.size();

    // Status
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusText, "OK");

    // Size
    EXPECT_EQ(resBodySize, fileGen.m_randomData.size() * 8);

    // Body
    for (size_t i = 0; i < fileGen.m_randomData.size(); i++) {
        uint64_t valToCompare = fileGen.m_randomData[i];
        for (int j = 0; j < 8; j++) {
            EXPECT_EQ(
                static_cast<char>(valToCompare & 0xFF),
                static_cast<char>(res.body[(i * 8) + j])
            );
            valToCompare >>= 8;
        }
    }
}

TEST(FileHandlerTest, ReadFromInvalidFile) {
    const std::string invalidFileName = "NonExistingFile.txt";
    HttpResponse res = FileHandler::MakeHttpResponseFromFile(invalidFileName);

    EXPECT_EQ(res.statusCode, 404);
    EXPECT_EQ(res.statusText, "Not Found");
    EXPECT_TRUE(res.body.empty());
}
