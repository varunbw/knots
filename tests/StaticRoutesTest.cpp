#include "knots/StaticRoutes.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>
#include <sys/types.h>

#include "knots/HttpMessage.hpp"
#include "knots/Router.hpp"
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
    static constexpr std::string_view m_commonFileContents = "Hello world, from file `{}`";

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
                filePath.string()
            ));

            continue;
        }

        std::string fileContents = std::format(
            m_commonFileContents,
            (path / m_commonFileName).string()
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


TEST(StaticRoutesTest, AddStaticFile) {

    DummyDirectoryGenerator ddg;
    Router router;

    const std::string relativeFilePath = ddg.m_paths[1] / ddg.m_commonFileName;
    const std::string absoluteFilePath = ddg.m_baseTestingDirectory / relativeFilePath;

    StaticRoutes::AddStaticFile(absoluteFilePath, router, ddg.m_baseTestingDirectory);

    HttpRequest req;
    req.method = HttpMethod::GET;
    req.requestUrl = "/" + relativeFilePath;

    const SegmentHandlerFunctions* handlers = router.FetchFunctionsForRoute(req);

    // Check that the segment exists
    EXPECT_NE(handlers, nullptr);

    const HandlerFunction& handler = handlers->GetHandler(req.method);

    // Check that the handler for GET exists
    EXPECT_NE(handler, nullptr);

    HttpResponse res;
    handler(req, res);

    // Check body content
    EXPECT_EQ(
        res.body, 
        std::format(
            ddg.m_commonFileContents,
            relativeFilePath
        )
    );

    // Non existent route
    // I smashed my head on the keyboard
    req.requestUrl = "/76ytuhg5yhg67tubjnyh76utgbjn8r54tfegd690oi";
    handlers = router.FetchFunctionsForRoute(req);

    EXPECT_EQ(handlers, nullptr);
}

TEST(StaticRoutesTest, AddStaticDirectory) {

    DummyDirectoryGenerator ddg;
    Router router;

    StaticRoutes::AddStaticDirectory(ddg.m_baseTestingDirectory, router, ddg.m_baseTestingDirectory);

    HttpRequest req;
    req.method = HttpMethod::GET;

    for (const fs::path& path : ddg.m_paths) {

        req.requestUrl = std::format(
            "/{}{}",
            path.string(), ddg.m_commonFileName.string()
        );

        Log::Warning(std::format(
            "req: `{}`",
            req.requestUrl
        ));

        const SegmentHandlerFunctions* handlers = router.FetchFunctionsForRoute(req);

        EXPECT_NE(handlers, nullptr);

        const HandlerFunction& handler = handlers->GetHandler(req.method);

        EXPECT_NE(handler, nullptr);

        HttpResponse res;
        handler(req, res);

        EXPECT_EQ(
            res.body,
            std::format(
                ddg.m_commonFileContents,
                (path / ddg.m_commonFileName).string()
            )
        );
    }

    // Non existent route
    // I smashed my head on the keyboard
    req.requestUrl = "/76ytuhg5yhg67tubjnyh76utgbjn8r54tfegd690oi";
    const SegmentHandlerFunctions* handlers = router.FetchFunctionsForRoute(req);

    EXPECT_EQ(handlers, nullptr);
}