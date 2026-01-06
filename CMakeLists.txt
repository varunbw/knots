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

target_compile_options(knots PRIVATE 
    -Wall      # Enable all compiler warnings
    -Wextra    # Enable extra compiler warnings
    -Wpedantic # Enable standard checking
    -fmax-errors=3 # Limit the number of errors shown
    -fno-diagnostics-show-template-tree # Disable template tree diagnostics
)

# Debug-specific flags
target_compile_options(knots PRIVATE
    $<$<CONFIG:Debug>:-g>
)

# Release-specific flags
target_compile_options(knots PRIVATE
    $<$<CONFIG:Release>:-O3>
)

target_include_directories(knots PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(knots PUBLIC yaml-cpp::yaml-cpp)

# Make example buld and tests only when making the library
if (PROJECT_IS_TOP_LEVEL)
    # Executable Target
    if (KNOTS_EXAMPLE_BUILD)
        if (${KNOTS_EXAMPLE_BUILD} STREQUAL "True")
            add_executable(knots-app examples/main.cpp)
            target_link_libraries(knots-app PRIVATE knots)

        target_compile_options(knots-app PRIVATE 
            -Wall      # Enable all compiler warnings
            -Wextra    # Enable extra compiler warnings
            -Wpedantic # Enable standard checking
            -fmax-errors=3 # Limit the number of errors shown
            -fno-diagnostics-show-template-tree # Disable template tree diagnostics
        )

        # Debug-specific flags
        target_compile_options(knots-app PRIVATE
            $<$<CONFIG:Debug>:-g>
        )

        # Release-specific flags
        target_compile_options(knots-app PRIVATE
            $<$<CONFIG:Release>:-O3>
        )
        endif()
    endif()

    # -------- TESTING --------
    # Enable testing
    enable_testing()

    # Download and install GoogleTest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)

    # Add test directory: tests/CMakeLists.txt
    add_subdirectory(tests)
endif()