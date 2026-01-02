cmake_minimum_required(VERSION 3.21)
project(knots)

include(FetchContent)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Download yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)

# Library Target
add_library(
    knots
    src/FileHandler.cpp
    src/HttpRequest.cpp
    src/HttpResponse.cpp
    src/HttpServer.cpp
    src/NetworkIO.cpp
    src/Router.cpp
    src/ThreadPool.cpp
    src/Utils.cpp
)

# Compiler options set in CMakePresets.json
    # -Wall   | Enable all compiler warnings
    # -Wextra | Enable extra compiler warnings
    # -fno-diagnostics-show-template-tree | Disable template tree diagnostics
    # -fmax-errors=3 | Limit the number of errors shown

    # Debug build specific
        # -g  | Add debug info

    # Release build specific
        # -O3 | Performance

if (KNOTS_COMPILER_FLAGS)
    separate_arguments(KNOTS_COMPILER_FLAGS_LIST NATIVE_COMMAND "${KNOTS_COMPILER_FLAGS}")
else()
    if (MSVC)
        set(KNOTS_COMPILER_FLAGS_LIST /W4)
    else()
        set(KNOTS_COMPILER_FLAGS_LIST -O3)
    endif()
endif()

target_compile_options(knots PRIVATE ${FLAGS_LIST})
target_link_options(knots PRIVATE ${FLAGS_LIST})

target_include_directories(knots PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(knots PUBLIC yaml-cpp::yaml-cpp)

# Make example buld and tests only when making the library
if (PROJECT_IS_TOP_LEVEL)
    # Executable Target
    if (KNOTS_EXAMPLE_BUILD)
        if (${KNOTS_EXAMPLE_BUILD} STREQUAL "True")
            add_executable(knots-app example/main.cpp)
            target_link_libraries(knots-app PRIVATE knots)
        endif()
    endif()

    # -------- TESTING --------
    # Enable testing
    enable_testing()

    # Download and install GoogleTest
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    # Add test directory: tests/CMakeLists.txt
    add_subdirectory(tests)
endif()