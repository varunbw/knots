# knots

A general-purpose multi-threaded HTTP server written in C++. Designed to handle HTTP messages I/O over the network, it provides functionality for concurrent networking and routing. knots gives you the ability to define your own functions for API endpoints, while also providing an easy file handler meant to deal with static files.

Currently works with the HTTP/1.1 version.

## Requirements
- C++20 or later
- CMake 3.20 or later

## Dependencies
- yaml-cpp
- GoogleTest

## Project Structure
- `config/` - Configuration files
- `include/` - Header files
- `src/` - Source files
    - `main.cpp` - Entry point of the server
    - `FileHandler.cpp` - Handles file reading logic
    - `HttpRequest.cpp` - Methods for `HttpRequest` struct and HTTP Request parsing
    - `HttpResponse.cpp` - Methods for `HttpResponse` struct and HTTP Response parsing
    - `HttpServer.cpp` - Main server implementation
    - `NetworkIO.cpp` - Network I/O operations
    - `Router.cpp` - URL routing logic
    - `ThreadPool.cpp` - Thread pool for request management
    - `Utils.cpp` - Utility functions
- `tests/` - Unit tests

# Installation
Quick-start snippet at the end of this document.

## Steps to Install
1. Ensure you have the dependencies installed on your system.
#### Ubuntu/Debian
```bash
sudo apt install libyaml-cpp-dev libgtest-dev
```

2. Clone the repository:
```bash
git clone https://github.com/varunbw/knots.git
cd knots
```

3. Create a build directory:
```bash
mkdir build
cd build
```

4. Run CMake to generate the Makefile:
```bash
cmake ..
```

5. Build the project:
```bash
make
```

6. Run the server after navigating to the root directory:
```bash
cd .. && ./knots
```


# Connecting to the server
You can connect to the server using a web browser or a tool like `curl`. The server will be running on `localhost` at port `8600` as specified in [config/main.conf](config/main.conf) by default.

```bash
curl http://localhost:8600
```
This should return a simple HTML page.


# Running Tests
This project uses GoogleTest for unit testing. The tests are located in the `tests` directory.
To run them, navigate to the `build` directory and run:
```bash
make test
```
This will execute all the tests defined in the `tests` directory.


# Configuration
The server can be configured using a YAML configuration file, located at `config/config.yaml`.

- `config/config.yaml` - Main server configuration for:
    - `port`: The port on which the server listens.
    - `max-connections`: Maximum number of concurrent connections the server can handle.
    - `run-console-input-thread`: Whether or not to run a thread for handling console input. This is usually set false only during running tests, since this thread gives you the ability to gracefully shutdown the server from the console by sending "stop", "exit", "quit" to `std::cin`
```yaml
port: 8600
max-connections: 10
run-console-input-thread: true
```

# Quick Start Snippet
```bash
sudo apt install libyaml-cpp-dev libgtest-dev
git clone https://github.com/varunbw/knots.git
cd knots
mkdir build && cd build
cmake .. && make && cd ..
./knots
```
