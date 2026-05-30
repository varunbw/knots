#include <filesystem>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>
#include <sys/types.h>

#include "knots/Utils.hpp"

namespace fs = std::filesystem;

class DummyDirectoryGenerator {
public:
    
    const fs::path m_baseTestingDirectory = "./StaticRoutesTest/";
    const std::vector<fs::path> m_paths = {
        "1/",
        "1/6/",
        "1/7/",
        "1/8/",
        "1/9/",
        "1/10/",
        "2/",
        "2/6/",
        "2/7/",
        "2/8/",
        "2/9/",
        "2/10/",
        "3/",
        "3/6/",
        "3/7/",
        "3/8/",
        "3/9/",
        "3/10/",
        "4/",
        "4/6/",
        "4/7/",
        "4/8/",
        "4/9/",
        "4/10/",
        "5/",
        "5/6/",
        "5/7/",
        "5/8/",
        "5/9/",
        "5/10/",
    };

    const fs::path m_commonFileName = "test-file.txt";

    DummyDirectoryGenerator();
    ~DummyDirectoryGenerator();
};


DummyDirectoryGenerator::DummyDirectoryGenerator() {

    if (fs::create_directory(m_baseTestingDirectory) == false) {
        Log::Error(std::format(
            "DummyDirectoryGenerator(): Failed to create directory `{}`",
            m_baseTestingDirectory.string()
        ));

        return;
    }

    for (const fs::path& path : m_paths) {
        const fs::path directoryName = m_baseTestingDirectory / path;

        if (fs::create_directory(directoryName) == false) {
            Log::Error(std::format(
                "DummyDirectoryGenerator(): Failed to create directory `{}`",
                directoryName.string()
            ));

            continue;
        }

        const fs::path filePath = directoryName / m_commonFileName;

        std::ofstream outfile(filePath);
        if (outfile.is_open() == false) {
            Log::Error(std::format(
                "Could not create file {}",
                (filePath).string()
            ));

            continue;
        }

        const std::string fileContents = std::format(
            "Hello world, from file {}",
            filePath.string()
        );
        
        outfile.write(
            reinterpret_cast<const char*>(fileContents.data()),
            fileContents.size()
        );
    }

    return;
}

DummyDirectoryGenerator::~DummyDirectoryGenerator() {

    if (fs::remove_all(m_baseTestingDirectory) == false) {
        Log::Error(std::format(
            "~DummyDirectoryGenerator(): Failed to delete `{}`",
            m_baseTestingDirectory.string()
        ));
    }
    
    return;
}


TEST(StaticRoutesTest, FullFunctionalityTest) {

    DummyDirectoryGenerator ddg;

}