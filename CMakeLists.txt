cmake_minimum_required(VERSION 3.20)
project(knots)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# Place the executable in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Add debug info
add_compile_options(-g)
# Enable all compiler warnings
add_compile_options(-Wall)
# Enable extra compiler warnings
add_compile_options(-Wextra)
# Disable template tree diagnostics
add_compile_options(-fno-diagnostics-show-template-tree)
# Limit the number of errors shown
add_compile_options(-fmax-errors=3)

# Add executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/HttpMessage.cpp
    src/HttpParser.cpp
    src/HttpServer.cpp
    src/Utils.cpp
)