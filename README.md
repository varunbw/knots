# knots

A general-purpose multi-threaded HTTP 1.1 server written in C++. Designed to handle HTTP messages I/O over the network, it provides functionality for concurrent networking and routing. knots gives you the ability to define your own functions for API endpoints, while also providing an easy file handler meant to deal with static files.

## Requirements
- C++20 or above
- CMake 3.21 or above
- Ninja

#### Ubuntu/Debian
```bash
sudo apt install cmake ninja-build
```

## Dependencies
These are automatically installed by CMake:
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [GoogleTest](https://github.com/google/googletest)

> When built as a library in your project, tests are NOT generated, and the GoogleTest library is also not fetched. Tests will only be made in library-development builds, when the `knots` project is top-level.

## Project Structure
- `config/` - Configuration files
- `examples/` - Examples of how to use the library
- `include/` - Header files
- `src/` - Source files
    - `FileHandler.cpp` - Handles file reading logic
    - `HttpRequest.cpp` - Methods for `HttpRequest` struct and HTTP Request parsing
    - `HttpResponse.cpp` - Methods for `HttpResponse` struct and HTTP Response building
    - `HttpServer.cpp` - Main server implementation
    - `NetworkIO.cpp` - Network I/O operations
    - `Router.cpp` - URL routing logic
    - `ThreadPool.cpp` - Thread pool for request management
    - `Utils.cpp` - Utility functions
- `tests/` - Unit tests

# Building
Quick-start snippet at the end of this document.

### Install and run a simple demo server
1. Clone the repository:
```bash
git clone https://github.com/varunbw/knots.git
cd knots
```

2. Generate a `debug` or `release` build with the example app provided in [examples/main.cpp](examples/main.cpp).
```bash
cmake --preset example-debug
cmake --build --preset example-debug
```

3. Run the server, it'll be up on port `8600` by default:
```bash
./build/example-debug/knots-app
```

# Using as a library in your project
Include this in your `CMakeLists.txt`
```cmake
FetchContent_Declare(
    knots
    GIT_REPOSITORY https://github.com/varunbw/knots.git
    GIT_TAG main # Change this to a specific commit if you'd prefer working on a proven stable build
)
FetchContent_MakeAvailable(knots)

target_link_libraries(your-library PUBLIC knots)
```

# Tests
This project uses [GoogleTest](https://github.com/google/googletest) for testing. Make a `debug` or `release` build to run tests.
```bash
cmake --preset debug && cmake --build --preset debug
ctest --preset tests-debug
```


# Configuration
The server can be configured using a YAML configuration file, located at `config/config.yaml` by default. You can edit the location of the file by specifying the file path in the source code (see [HttpServerConfiguration](src/Utils.cpp) and the [example](examples/main.cpp)).

- `config/config.yaml` - Main server configuration for:
    - `port`: The port on which the server listens.
    - `max-connections`: Maximum number of concurrent connections the server can handle.
    - `input-polling-interval-ms`: How frequently the thread responsible for handling console input should check for an user input. (In tests, this is set to `0` in the source files to avoid stalling the tests)
```yaml
port: 8600
max-connections: 125
input-polling-interval-ms: 5000
```

# Quick Start Snippet
```bash
git clone https://github.com/varunbw/knots.git
cd knots
cmake --preset example-debug
cmake --build --preset example-debug
./build/example-debug/knots-app
```