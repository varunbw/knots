cmake_minimum_required(VERSION 3.20)
project(knots)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
    # Add debug info
    # Enable all compiler warnings
    # Enable extra compiler warnings
    # Disable template tree diagnostics
    # Limit the number of errors shown
add_compile_options(-g -Wall -Wextra -fno-diagnostics-show-template-tree -fmax-errors=3)

# Place the executable in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Dependencies
find_package(yaml-cpp REQUIRED)

# Library Target
add_library(
    knots_lib
    src/FileHandler.cpp
    src/HttpBuilder.cpp
    src/HttpMessage.cpp
    src/HttpParser.cpp
    src/HttpServer.cpp
    src/NetworkIO.cpp
    src/Router.cpp
    src/ThreadPool.cpp
    src/Utils.cpp
)

target_include_directories(knots_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(knots_lib PUBLIC yaml-cpp)

# Executable Target
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE knots_lib)



# -------- TESTING --------

# Enable testing
enable_testing()

# Download and install GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add test directory: tests/CMakeLists.txt
add_subdirectory(tests)

# Copy configuration files to the build directory
configure_file(${CMAKE_SOURCE_DIR}/config/routes.yaml ${CMAKE_BINARY_DIR}/config/routes.yaml COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config/main.conf ${CMAKE_BINARY_DIR}/config/main.conf COPYONLY)