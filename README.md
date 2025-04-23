# knots
knots is a multithreaded HTTP server written in C++. It is designed to handle multiple requests concurrently using threads, and can serve static files.


## Features
- Multithreaded architecture
- Handles multiple requests concurrently
- Serves static files

# Installation
TLDR: Quick-start snippet at the end of this document.
To build the project, you need to have CMake installed. Follow these steps:
1. Clone the repository:
```bash
git clone https://github.com/varunbw/knots.git
cd knots
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Run CMake to generate the Makefile:
```bash
cmake ..
```

4. Build the project:
```bash
make
```

5. Run the server after navigating to the root directory:
```bash
./knots
```


# Quick Start Snippet
Just copy paste this snippet in your terminal to get started quickly:
```bash
git clone https://github.com/varunbw/knots.git
cd knots
mkdir build && cd build
cmake .. && make && cd ..
./knots
```
