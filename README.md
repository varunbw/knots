# knots
knots is a multithreaded HTTP server written in C++. It is designed to handle multiple requests concurrently using threads, and can serve static files.

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
    - `HttpBuilder.cpp` - HTTP response building logic
    - `HttpMessage.cpp` - Some utility functions for HTTP messages
    - `HttpParser.cpp` - HTTP Request parsing
    - `HttpServer.cpp` - Main server implementation
    - `NetworkIO.cpp` - Network I/O operations
    - `Router.cpp` - URL routing logic
    - `ThreadPool.cpp` - Thread pool for request management
    - `Utils.cpp` - Utility functions
- `static/` - Static files served by the server
- `tests/` - Unit tests

# Installation
Quick-start snippet at the end of this document.

## Steps to Install
1. Ensure you have the dependencies installed on your system.
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
./knots
```


# Connecting to the server
You can connect to the server using a web browser or a tool like `curl`. The server will be running on `localhost` at port `8686` as specified in [config/main.conf](config/main.conf) by default.

```bash
curl http://localhost:8686
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
    - `routes-path`: Path to the routes configuration file.
    - `max-connections`: Maximum number of concurrent connections the server can handle.
```yaml
port: 8686
routes-path: config/routes.yaml
max-connections: 10
```

- `config/routes.yaml` - Routes configuration for the server. (The type field currently does nothing :))
```yaml
routes:
  /:
    file: static/index.html
    type: text/html
  /index.html:
    file: static/index.html
    type: text/html
  /test.html:
    file: static/test.html
    type: text/html
  /favicon.ico:
    file: static/knots-icon.jpg
    type: image/jpeg
```


# Quick Start Snippet
You can copy paste this snippet in your terminal to get started quickly:
```bash
sudo apt install libyaml-cpp-dev libgtest-dev
git clone https://github.com/varunbw/knots.git
cd knots
mkdir build && cd build
cmake .. && make && cd ..
./knots
```
