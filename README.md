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
- [GoogleTest](https://github.com/google/googletest)

GoogleTest is only fetched and built when you are developing the knots library itself. If you're using `knots` in your project as a library, GoogleTest won't be fetched, in order to avoid clutter in your dependencies. (I shall not merge untested code to main, trust)

## Project Structure
- `examples/` - Examples of how to use the library
- `include/` - Header files
- `src/` - Source files
    - [FileHandler.cpp](./src/FileHandler.cpp) - Handles file reading logic
    - [HttpRequest.cpp](./src/HttpRequest.cpp) - Methods for `HttpRequest` struct and HTTP Request parsing
    - [HttpResponse.cpp](./src/HttpResponse.cpp) - Methods for `HttpResponse` struct and HTTP Response building
    - [HttpServer.cpp](./src/HttpServer.cpp) - Main server implementation
    - [NetworkIO.cpp](./src/NetworkIO.cpp) - Network I/O operations
    - [Router.cpp](./src/Router.cpp) - URL routing logic
    - [StaticRoutes.cpp](./src/StaticRoutes.cpp) - Utility for managing the routing for static files
    - [ThreadPool.cpp](./src/ThreadPool.cpp) - Thread pool for request management
    - `utils/` - Utility stuff
        - [Log.cpp](./src/utils/Log.cpp) - Logging functions
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

The server is configured using a struct passed to it, provided in [Config.hpp](./include/knots/utils/Config.hpp), with the following parameters:

- `port`: The port on which the server listens.
- `maxConnections`: Maximum number of concurrent connections the server handles.
- `inputPollingIntervalMs`: The interval (in milliseconds) at which the thread responsible for handling console input should check for an user input. (In tests, this is set to `0` avoid stalling them)

For simple cases, you can pass the values in the source code itself, and mark them as `constexpr` to avail compile-time optimization.

```c++
constexpr int port = 8600;
constexpr int maxConnections = 125;
constexpr int inputPollingIntevalMs = 100;
constexpr RequestLoggingVerbosity verbosity = RequestLoggingVerbosity::FULL;

constexpr HttpServerConfiguration config (
    port, maxConnections, inputPollingIntevalMs, verbosity
);

HttpServer server(config, router);
```

However it's recommended to use configuration files and then parse the values into the server configuration at runtime, this is standard practice. I did knot provide defualt configuration format and libraries for the purpose of flexibility; you can choose what file format and libraries to use.

# Commands
Currently, the server supports the following commands from standard console input:\
`q`, `quit`, `stop`, `exit`: Stop the server

# Quick Start Snippet
```bash
git clone https://github.com/varunbw/knots.git
cd knots
cmake --preset example-debug
cmake --build --preset example-debug
./build/example-debug/knots-app
```