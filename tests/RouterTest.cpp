#include <fstream>
#include <gtest/gtest.h>

#include "Router.hpp"

/*
    @brief Unit tests for the Router class
    
    This test suite uses Google Test framework
*/
class RouterTest : public ::testing::Test {
protected:
    const std::string validConfigPath = "tests/test_routes.yaml";
    const std::string invalidConfigPath = "tests/invalid_config.yaml";
    
    void SetUp() override {
        // Create valid config file
        std::ofstream configFile(validConfigPath);
        configFile << "routes:\n"
                  << "  /: \n"
                  << "    file: static/index.html\n"
                  << "    type: text/html\n"
                  << "  /index.html: \n"
                  << "    file: static/index.html\n"
                  << "    type: text/html\n"
                  << "  /test.html: \n"
                  << "    file: static/test.html\n"
                  << "    type: text/html\n"
                  << "  /favicon.ico: \n"
                  << "    file: static/knots-icon.jpg\n"
                  << "    type: image/jpeg\n";
        configFile.close();

        // Create invalid config file
        std::ofstream invalidConfigFile(invalidConfigPath);
        invalidConfigFile << "invalid_yaml\n";
        invalidConfigFile.close();
    }

    // Clean up the config files after tests
    void TearDown() override {
        remove(validConfigPath.c_str());
        remove(invalidConfigPath.c_str());
    }
};

/*
    @brief Test if the router can load routes from a valid config file
*/
TEST_F(RouterTest, LoadRoutesFromConfig_ValidConfig) {
    EXPECT_NO_THROW({
        Router router(validConfigPath);
    });
}

/*
    @brief Test if the router throws an error when loading routes from an invalid config file
*/
TEST_F(RouterTest, LoadRoutesFromConfig_InvalidConfig) {
    EXPECT_THROW({
        Router router(invalidConfigPath);
    }, std::runtime_error);
}

/*
    @brief Test if the router can return a valid route for a given request URL
*/
TEST_F(RouterTest, GetRoute_ValidRoute) {
    Router router(validConfigPath);
    Route route = router.GetRoute("/");
    
    EXPECT_TRUE(route.IsValid());
    EXPECT_EQ(route.filePath, "static/index.html");
    EXPECT_EQ(route.contentType, "text/html");
}

/*
    @brief Test if the router returns an invalid route for a non-existent request URL
*/
TEST_F(RouterTest, GetRoute_InvalidRoute) {
    Router router(validConfigPath);
    Route route = router.GetRoute("/invalid-route");
    
    EXPECT_FALSE(route.IsValid());
}