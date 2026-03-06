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

