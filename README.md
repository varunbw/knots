# knots
knots is a multithreaded HTTP server written in C++. It is designed to handle multiple requests concurrently using threads, and can serve static files.


## Features
- Multithreaded architecture
- Handles multiple requests concurrently
- Serves static files

# Installation
Quick-start snippet at the end of this document.

## Dependencies
- C++20 or later
- CMake 3.20 or later
- yaml-cpp

## Steps to Install

1. Ensure you have C++20, CMake, and yaml-cpp installed on your system.
```bash
sudo apt install cmake libyaml-cpp-dev
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


# Quick Start Snippet
Just copy paste this snippet in your terminal to get started quickly:
```bash
sudo apt install libyaml-cpp-dev
git clone https://github.com/varunbw/knots.git
cd knots
mkdir build && cd build
cmake .. && make && cd ..
./knots
```
